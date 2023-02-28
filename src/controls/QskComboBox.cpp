/******************************************************************************
 * QSkinny - Copyright (C) 2023 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskComboBox.h"

#include "QskGraphic.h"
#include "QskMenu.h"
#include "QskTextOptions.h"

#include <QGuiApplication>

QSK_SUBCONTROL( QskComboBox, Panel )
QSK_SUBCONTROL( QskComboBox, Graphic )
QSK_SUBCONTROL( QskComboBox, Text )
QSK_SUBCONTROL( QskComboBox, OpenMenuGraphic )
QSK_SUBCONTROL( QskComboBox, Ripple )

QSK_SYSTEM_STATE( QskComboBox, Pressed, QskAspect::FirstSystemState << 1 )
QSK_SYSTEM_STATE( QskComboBox, PopupOpen, QskAspect::FirstSystemState << 2 )

class QskComboBox::PrivateData
{
  public:
    PrivateData( QskComboBox* const box )
        : menu( new QskMenu( box ) )
    {
        menu->setPopupFlag( QskPopup::DeleteOnClose, false );
    }

    QskMenu* const menu;
    QString label;
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

    connect( m_data->menu, &QskMenu::currentIndexChanged, this, &QskComboBox::currentIndexChanged );
    connect( m_data->menu, &QskMenu::currentIndexChanged, this, &QQuickItem::update );

    connect( m_data->menu, &QskMenu::countChanged, this, &QskComboBox::countChanged );

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
    const int index = m_data->menu->currentIndex();

    if( index >= 0 )
    {
        const auto option = m_data->menu->optionAt( index );
        return option.at( 0 ).value< QskGraphic >();
    }
    else
    {
        return {};
    }
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

QString QskComboBox::label() const
{
    return m_data->label;
}

void QskComboBox::setLabel( const QString& label )
{
    m_data->label = label;
}

QString QskComboBox::text() const
{
    const int index = m_data->menu->currentIndex();

    if( index >= 0 )
    {
        const auto option = m_data->menu->optionAt( index );
        return option.at( 1 ).toString();
    }
    else
    {
        return label();
    }
}

void QskComboBox::togglePopup()
{
    setPopupOpen( !isPopupOpen() );
}

void QskComboBox::openPopup()
{
    m_data->menu->open();
}

void QskComboBox::closePopup()
{
    m_data->menu->close();
}

void QskComboBox::updateLayout()
{
    Inherited::updateLayout();

    auto origin = contentsRect().bottomLeft();
    m_data->menu->setOrigin( origin );

    m_data->menu->setFixedWidth( contentsRect().width() );
}

void QskComboBox::mousePressEvent( QMouseEvent* event )
{
    Q_UNUSED( event )
    setPressed( true );
}

void QskComboBox::mouseReleaseEvent( QMouseEvent* event )
{
    Q_UNUSED( event )
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

            // always accepting
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

void QskComboBox::click()
{
    setPressed( true );
    releaseButton();
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
