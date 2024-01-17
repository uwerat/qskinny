/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskComboBox.h"

#include "QskLabelData.h"
#include "QskMenu.h"
#include "QskTextOptions.h"
#include "QskEvent.h"

#include <qpointer.h>
#include <qquickwindow.h>

QSK_SUBCONTROL( QskComboBox, Panel )
QSK_SUBCONTROL( QskComboBox, Icon )
QSK_SUBCONTROL( QskComboBox, Text )
QSK_SUBCONTROL( QskComboBox, StatusIndicator )

QSK_SYSTEM_STATE( QskComboBox, Pressed, QskAspect::FirstSystemState << 1 )
QSK_SYSTEM_STATE( QskComboBox, PopupOpen, QskAspect::FirstSystemState << 2 )

static inline void qskTraverseOptions( QskComboBox* comboBox, int steps )
{
    const auto count = comboBox->count();
    if ( count == 0 )
        return;

    const int index = comboBox->currentIndex();

    if ( index == -1 && steps < 0 )
        steps++;

    int nextIndex = ( index + steps ) % count;
    if ( nextIndex < 0 )
        nextIndex += count;

    if ( nextIndex != index )
        comboBox->setCurrentIndex( nextIndex );
}

static inline int qskFindOption( QskComboBox* comboBox, const QString& key )
{
    if ( !key.isEmpty() )
    {
        const int currentIndex = comboBox->currentIndex();
        const int count = comboBox->count();

        for ( int i = 1; i < count; i++ )
        {
            const int index = ( currentIndex + i ) % count;
            const auto text = comboBox->textAt( index );

            if ( text.startsWith( key ) )
                return index;
        }
    }

    return -1;
}

class QskComboBox::PrivateData
{
  public:
    QPointer < QskMenu > menu;

    QVector< QskLabelData > options;
    QString placeholderText;

    int currentIndex = -1;
};

QskComboBox::QskComboBox( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData() )
{
    initSizePolicy( QskSizePolicy::Minimum, QskSizePolicy::Fixed );

    setPolishOnResize( true );

    setAcceptedMouseButtons( Qt::LeftButton );
    setWheelEnabled( true );
    setFocusPolicy( Qt::StrongFocus );

    setAcceptHoverEvents( true );
}

QskComboBox::~QskComboBox()
{
}

bool QskComboBox::isPressed() const
{
    return hasSkinState( Pressed );
}

void QskComboBox::setPressed( bool on )
{
    if ( on == isPressed() )
        return;

    setSkinStateFlag( Pressed, on );
    Q_EMIT pressedChanged( on );

    if ( on )
        Q_EMIT pressed();
    else
        Q_EMIT released();
}

void QskComboBox::setPopupOpen( bool on )
{
    if ( on == isPopupOpen() )
        return;

    setSkinStateFlag( PopupOpen, on );

    if( on )
        openPopup();
    else
        closePopup();
}

bool QskComboBox::isPopupOpen() const
{
    return hasSkinState( PopupOpen );
}

void QskComboBox::setTextOptions( const QskTextOptions& textOptions )
{
    setTextOptionsHint( Text, textOptions );
}

QskTextOptions QskComboBox::textOptions() const
{
    return textOptionsHint( Text );
}

int QskComboBox::addOption( const QString& graphicSource, const QString& text )
{
    return addOption( QskLabelData( text, QskIcon( graphicSource ) ) );
}

int QskComboBox::addOption( const QUrl& graphicSource, const QString& text )
{
    return addOption( QskLabelData( text, QskIcon( graphicSource ) ) );
}

int QskComboBox::addOption( const QskLabelData& option )
{
    m_data->options += option;

    resetImplicitSize();
    update();

    if ( isComponentComplete() )
        Q_EMIT optionsChanged();

    return count() - 1;
}

void QskComboBox::setOptions( const QStringList& options )
{
    setOptions( qskCreateLabelData( options ) );
}

void QskComboBox::setOptions( const QVector< QskLabelData >& options )
{
    if ( options == m_data->options )
        return;

    m_data->options = options;
    m_data->currentIndex = -1; // currentIndexChanged ???

    resetImplicitSize();
    update();

    Q_EMIT optionsChanged();
}

QVector< QskLabelData > QskComboBox::options() const
{
    return m_data->options;
}

QskLabelData QskComboBox::optionAt( int index ) const
{
    return m_data->options.value( index );
}

QString QskComboBox::placeholderText() const
{
    return m_data->placeholderText;
}

void QskComboBox::setPlaceholderText( const QString& text )
{
    if ( m_data->placeholderText == text )
        return;

    m_data->placeholderText = text;
    resetImplicitSize();

    if ( m_data->currentIndex < 0 )
        update();

    Q_EMIT placeholderTextChanged( text );
}

QString QskComboBox::textAt( int index ) const
{
    return optionAt( index ).text();
}

QString QskComboBox::currentText() const
{
    if( m_data->currentIndex >= 0 && m_data->currentIndex < m_data->options.count() )
        return m_data->options[ m_data->currentIndex ].text();

    return m_data->placeholderText;
}

void QskComboBox::openPopup()
{
    /*
        maybe we should implement an alternative implementation
        using a QskSelectionDialog, that could be en/disabled
        by setting a mode TODO ...
     */

    if ( m_data->menu )
        return;

    const auto cr = contentsRect();

    auto menu = new QskMenu();

    menu->setParent( this );
    menu->setParentItem( window()->contentItem() );
    menu->setPopupFlag( QskPopup::DeleteOnClose, true );

    menu->setOrigin( mapToScene( cr.bottomLeft() ) );
    menu->setFixedWidth( cr.width() );

    menu->setOptions( m_data->options );

    connect( menu, &QskMenu::currentIndexChanged,
        this, &QskComboBox::indexInPopupChanged );

    connect( menu, &QskMenu::triggered,
        this, &QskComboBox::setCurrentIndex );

    connect( menu, &QskMenu::closed, this,
        [ this ]() { setPopupOpen( false ); setFocus( true ); } );

    m_data->menu = menu;
    menu->open();
}

void QskComboBox::closePopup()
{
    if ( m_data->menu )
        m_data->menu->close();
}

void QskComboBox::mousePressEvent( QMouseEvent* )
{
    setPressed( true );
    setPopupOpen( true );
}

void QskComboBox::mouseReleaseEvent( QMouseEvent* )
{
    setPressed( false );
}

void QskComboBox::keyPressEvent( QKeyEvent* event )
{
    if ( qskIsButtonPressKey( event ) )
    {
        if ( !event->isAutoRepeat() )
        {
            setPressed( true );
            setPopupOpen( true );
            return;
        }
    }

    switch( event->key() )
    {
#if 0
        case Qt::Key_F4:
        {
            // QComboBox does this ???
            setPressed( true );
            setPopupOpen( true );
            return;
        }
#endif
        case Qt::Key_Up:
        case Qt::Key_PageUp:
        {
            qskTraverseOptions( this, -1 );
            return;
        }
        case Qt::Key_Down:
        case Qt::Key_PageDown:
        {
            qskTraverseOptions( this, 1 );
            return;
        }
        case Qt::Key_Home:
        {
            if ( count() > 0 )
                setCurrentIndex( 0 );
            return;
        }
        case Qt::Key_End:
        {
            if ( count() > 0 )
                setCurrentIndex( count() - 1 );
            return;
        }

        default:
        {
            const int index = qskFindOption( this, event->text() );
            if ( index >= 0 )
            {
                setCurrentIndex( index );
                return;
            }
        }
    }

    Inherited::keyPressEvent( event );
}

void QskComboBox::keyReleaseEvent( QKeyEvent* event )
{
    Inherited::keyReleaseEvent( event );
}

void QskComboBox::wheelEvent( QWheelEvent* event )
{
    if ( isPopupOpen() )
    {
        if ( m_data->menu )
        {
#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
            auto wheelEvent = new QWheelEvent(
                event->position(), event->globalPosition(),
                event->pixelDelta(), event->angleDelta(),
                event->buttons(), event->modifiers(),
                event->phase(), event->inverted(), event->source() );
#else
            auto wheelEvent = event->clone();
#endif
            QCoreApplication::postEvent( m_data->menu, wheelEvent );
        }
    }
    else
    {
        const auto steps = -qRound( qskWheelSteps( event ) );
        qskTraverseOptions( this, steps );
    }
}

void QskComboBox::clear()
{
    if ( !m_data->options.isEmpty() )
    {
        m_data->options.clear();

        Q_EMIT optionsChanged();

        if ( m_data->currentIndex >= 0 )
        {
            m_data->currentIndex = -1;
            Q_EMIT currentIndexChanged( m_data->currentIndex );
        }

        update();
    }
}

void QskComboBox::setCurrentIndex( int index )
{
    if ( m_data->currentIndex != index )
    {
        m_data->currentIndex = index;
        update();

        Q_EMIT currentIndexChanged( index );
    }
}

int QskComboBox::currentIndex() const
{
    return m_data->currentIndex;
}

int QskComboBox::count() const
{
    return m_data->options.count();
}

int QskComboBox::indexInPopup() const
{
    if ( m_data->menu )
        return m_data->menu->currentIndex();

    return -1;
}

#include "moc_QskComboBox.cpp"
