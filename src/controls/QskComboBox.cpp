/******************************************************************************
 * QSkinny - Copyright (C) 2023 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskComboBox.h"

#include "QskGraphicProvider.h"
#include "QskGraphic.h"
#include "QskMenu.h"
#include "QskTextOptions.h"
#include "QskEvent.h"

#include <qpointer.h>
#include <qquickwindow.h>

QSK_SUBCONTROL( QskComboBox, Panel )
QSK_SUBCONTROL( QskComboBox, Graphic )
QSK_SUBCONTROL( QskComboBox, Text )
QSK_SUBCONTROL( QskComboBox, PopupIndicator )

QSK_SYSTEM_STATE( QskComboBox, PopupOpen, QskAspect::FirstSystemState << 1 )

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

namespace
{
    class Option
    {
      public:
        Option( const QskGraphic& graphic, const QString& text )
            : text( text )
            , graphic( graphic )
        {
        }

        Option( const QUrl& graphicSource, const QString& text )
            : graphicSource( graphicSource )
            , text( text )
        {
#if 1
            // lazy loading TODO ...
            if( !graphicSource.isEmpty() )
                graphic = Qsk::loadGraphic( graphicSource );
#endif
        }

        QUrl graphicSource;
        QString text;

        QskGraphic graphic;
    };
}

class QskComboBox::PrivateData
{
  public:
    QPointer < QskPopup > menu;

    QVector< Option > options;
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

QskGraphic QskComboBox::graphic() const
{
    if( m_data->currentIndex >= 0 )
    {
        const auto option = optionAt( m_data->currentIndex );
        return option.at( 0 ).value< QskGraphic >();
    }

    return QskGraphic();
}

void QskComboBox::setTextOptions( const QskTextOptions& textOptions )
{
    setTextOptionsHint( Text, textOptions );
}

QskTextOptions QskComboBox::textOptions() const
{
    return textOptionsHint( Text );
}

void QskComboBox::addOption( const QString& text )
{
    addOption( QUrl(), text );
}

void QskComboBox::addOption( const QskGraphic& graphic, const QString& text )
{
    m_data->options += Option( graphic, text );

    resetImplicitSize();
    update();

    if ( isComponentComplete() )
        Q_EMIT countChanged( count() );
}

void QskComboBox::addOption( const QString& graphicSource, const QString& text )
{
    addOption( QUrl( graphicSource ), text );
}

void QskComboBox::addOption( const QUrl& graphicSource, const QString& text )
{
    m_data->options += Option( graphicSource, text );

    resetImplicitSize();
    update();

    if ( isComponentComplete() )
        Q_EMIT countChanged( count() );
}

QVariantList QskComboBox::optionAt( int index ) const
{
    const auto& options = m_data->options;

    if( index < 0 || index >= options.count() )
        return QVariantList();

    const auto& option = options[ index ];

    QVariantList list;
    list += QVariant::fromValue( option.graphic );
    list += QVariant::fromValue( option.text );

    return list;
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
    if ( index >= 0 && index < m_data->options.count() )
        return m_data->options[ index ].text;

    return QString();
}

QString QskComboBox::currentText() const
{
    if( m_data->currentIndex >= 0 )
        return m_data->options[ m_data->currentIndex ].text;

    return m_data->placeholderText;
}

void QskComboBox::openPopup()
{
    if ( m_data->menu )
        return;

    const auto cr = contentsRect();

    auto menu = new QskMenu();

    menu->setParent( this );
    menu->setParentItem( window()->contentItem() );
    menu->setPopupFlag( QskPopup::DeleteOnClose, true );

    menu->setOrigin( mapToScene( cr.bottomLeft() ) );
    menu->setFixedWidth( cr.width() );

    for ( const auto& option : m_data->options )
        menu->addOption( option.graphic, option.text );

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
    setPopupOpen( true );
}

void QskComboBox::mouseReleaseEvent( QMouseEvent* )
{
}

void QskComboBox::keyPressEvent( QKeyEvent* event )
{
    if ( qskIsButtonPressKey( event ) )
    {
        if ( !event->isAutoRepeat() )
        {
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
            QCoreApplication::postEvent( m_data->menu, event->clone() );
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

        if ( isComponentComplete() )
            Q_EMIT countChanged( count() );
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

#include "moc_QskComboBox.cpp"
