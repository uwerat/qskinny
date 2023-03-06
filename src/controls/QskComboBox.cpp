/******************************************************************************
 * QSkinny - Copyright (C) 2023 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskComboBox.h"

#include "QskGraphic.h"
#include "QskMenu.h"
#include "QskTextOptions.h"

#include <qquickwindow.h>

QSK_SUBCONTROL( QskComboBox, Panel )
QSK_SUBCONTROL( QskComboBox, Graphic )
QSK_SUBCONTROL( QskComboBox, Text )
QSK_SUBCONTROL( QskComboBox, OpenMenuGraphic )
QSK_SUBCONTROL( QskComboBox, Splash )

QSK_SYSTEM_STATE( QskComboBox, Pressed, QskAspect::FirstSystemState << 1 )
QSK_SYSTEM_STATE( QskComboBox, PopupOpen, QskAspect::FirstSystemState << 2 )

class QskComboBox::PrivateData
{
  public:
    PrivateData( QskComboBox* const box )
        : menu( new QskMenu( box ) )
    {
        menu = new QskMenu();

        /*
            The popup menu is supposed to be modal for the window and
            therefore needs the root item of the window as parent item.
            So we set the box as QObject parent only, so that it gets
            destroyed properly.
         */
        menu->setParent( box );
        menu->setPopupFlag( QskPopup::DeleteOnClose, false );
    }

    QskMenu* menu;
    QString placeholderText;
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

    connect( m_data->menu, &QskMenu::currentIndexChanged,
        this, &QskComboBox::currentIndexChanged );

    connect( m_data->menu, &QskMenu::currentIndexChanged,
        this, &QQuickItem::update );

    connect( m_data->menu, &QskMenu::countChanged,
        this, &QskComboBox::countChanged );

    connect( this, &QskComboBox::currentIndexChanged,
        this, &QskControl::focusIndicatorRectChanged );

    connect( m_data->menu, &QskMenu::closed, this,
        [ this ]() { setPopupOpen( false ); setFocus( true ); } );

    connect( this, &QskComboBox::pressed, this, &QskComboBox::togglePopup );
}

QskComboBox::~QskComboBox()
{
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

bool QskComboBox::isPressed() const
{
    return hasSkinState( Pressed );
}

void QskComboBox::setPopupOpen( bool on )
{
    if ( on == isPopupOpen() )
        return;

    if( on )
    {
        openPopup();
    }
    else
    {
        closePopup();
    }

    setSkinStateFlag( PopupOpen, on );
    Q_EMIT popupOpenChanged( on );
}

bool QskComboBox::isPopupOpen() const
{
    return hasSkinState( PopupOpen );
}

QskGraphic QskComboBox::graphic() const
{
    const int index = currentIndex();
    if( index >= 0 )
    {
        const auto option = m_data->menu->optionAt( index );
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

    if ( currentIndex() < 0 )
        update();

    Q_EMIT placeholderTextChanged( text );
}

QString QskComboBox::text() const
{
    const int index = currentIndex();
    if( index >= 0 )
    {
        const auto option = optionAt( index );
        return option.at( 1 ).toString();
    }

    return placeholderText();
}

void QskComboBox::togglePopup()
{
    setPopupOpen( !isPopupOpen() );
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
    setPressed( true );
}

void QskComboBox::mouseUngrabEvent()
{
    setPressed( false );
}

void QskComboBox::mouseReleaseEvent( QMouseEvent* )
{
    releaseButton();
}

void QskComboBox::keyPressEvent( QKeyEvent* event )
{
    switch ( event->key() )
    {
        case Qt::Key_Select:
        case Qt::Key_Space:
        {
            if ( !event->isAutoRepeat() )
            {
                setPressed( true );
                // calling release button here, because
                // we will never get the key release event
                // when the menu is opened:
                releaseButton();
            }

            return;
        }
    }

    Inherited::keyPressEvent( event );
}

void QskComboBox::keyReleaseEvent( QKeyEvent* event )
{
    Inherited::keyReleaseEvent( event );
}

void QskComboBox::clear()
{
    m_data->menu->clear();
    update();
}

void QskComboBox::setCurrentIndex( int index )
{
    m_data->menu->setCurrentIndex( index );
}

int QskComboBox::currentIndex() const
{
    return m_data->menu->currentIndex();
}

int QskComboBox::count() const
{
    return m_data->menu->count();
}

void QskComboBox::releaseButton()
{
    if ( !isPressed() )
        return;

    setPressed( false );
    Q_EMIT clicked();
}

#include "moc_QskComboBox.cpp"
