/******************************************************************************
 * QSkinny - Copyright (C) 2023 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskComboBox.h"

#include "QskGraphic.h"
#include "QskMenu.h"
#include "QskTextOptions.h"
#include "QskEvent.h"

#include <qquickwindow.h>

QSK_SUBCONTROL( QskComboBox, Panel )
QSK_SUBCONTROL( QskComboBox, Graphic )
QSK_SUBCONTROL( QskComboBox, Text )
QSK_SUBCONTROL( QskComboBox, PopupIndicator )

QSK_SYSTEM_STATE( QskComboBox, PopupOpen, QskAspect::FirstSystemState << 1 )

static inline void qskIncrement( QskComboBox* comboBox, int steps )
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

class QskComboBox::PrivateData
{
  public:
    PrivateData( QskComboBox* comboBox )
    {
        menu = new QskMenu();

        /*
            The popup menu is supposed to be modal for the window and
            therefore needs the root item of the window as parent item.
            So we set the box as QObject parent only, so that it gets
            destroyed properly.
         */
        menu->setParent( comboBox );
        menu->setPopupFlag( QskPopup::DeleteOnClose, false );
    }

    QskMenu* menu;
    QString placeholderText;

    int currentIndex = -1;
};

QskComboBox::QskComboBox( QQuickItem* parent )
    : Inherited( parent )
    , m_data( new PrivateData( this ) )
{
    initSizePolicy( QskSizePolicy::Minimum, QskSizePolicy::Fixed );

    setPolishOnResize( true );

    setAcceptedMouseButtons( Qt::LeftButton );
    setWheelEnabled( true );
    setFocusPolicy( Qt::StrongFocus );

    setAcceptHoverEvents( true );

    connect( m_data->menu, &QskMenu::triggered,
        this, &QskComboBox::setCurrentIndex );

    connect( m_data->menu, &QskMenu::countChanged,
        this, &QskComboBox::countChanged );

    connect( m_data->menu, &QskMenu::closed, this,
        [ this ]() { setPopupOpen( false ); setFocus( true ); } );
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

void QskComboBox::addOption( const QUrl& graphicSource, const QString& text )
{
    m_data->menu->addOption( graphicSource, text );
}

QVariantList QskComboBox::optionAt( int index ) const
{
    return m_data->menu->optionAt( index );
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

QString QskComboBox::currentText() const
{
    if( m_data->currentIndex >= 0 )
    {
        const auto option = optionAt( m_data->currentIndex );
        return option.at( 1 ).toString();
    }

    return placeholderText();
}

void QskComboBox::openPopup()
{
    const auto cr = contentsRect();

    auto menu = m_data->menu;

    menu->setParentItem( window()->contentItem() );
    menu->setOrigin( mapToScene( cr.bottomLeft() ) );
    menu->setFixedWidth( cr.width() );

    menu->open();
}

void QskComboBox::closePopup()
{
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
            qskIncrement( this, -1 );
            return;
        }
        case Qt::Key_Down:
        case Qt::Key_PageDown:
        {
            qskIncrement( this, 1 );
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
            // searching option by key TODO ...
            break;
    }

    Inherited::keyPressEvent( event );
}

void QskComboBox::keyReleaseEvent( QKeyEvent* event )
{
    Inherited::keyReleaseEvent( event );
}

void QskComboBox::wheelEvent( QWheelEvent* event )
{
    if ( !isPopupOpen() )
    {
        const auto steps = -qRound( qskWheelSteps( event ) );
        qskIncrement( this, steps );
    }
}

void QskComboBox::clear()
{
    m_data->menu->clear();
    setCurrentIndex( -1 );
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
    return m_data->menu->count();
}

#include "moc_QskComboBox.cpp"
