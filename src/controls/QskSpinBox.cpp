/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskSpinBox.h"
#include "QskIntervalF.h"
#include "QskEvent.h"

QSK_SUBCONTROL( QskSpinBox, Panel )

QSK_SUBCONTROL( QskSpinBox, TextPanel )
QSK_SUBCONTROL( QskSpinBox, Text )

QSK_SUBCONTROL( QskSpinBox, IncrementPanel )
QSK_SUBCONTROL( QskSpinBox, IncrementIndicator )

QSK_SUBCONTROL( QskSpinBox, DecrementPanel )
QSK_SUBCONTROL( QskSpinBox, DecrementIndicator )

QSK_SYSTEM_STATE( QskSpinBox, Pressed, ( QskAspect::QskAspect::FirstSystemState << 0 ) )

namespace
{
    enum
    {
        ButtonDecrement = -1,
        ButtonNone      = 0,
        ButtonIncrement = 1
    };

    inline int buttonAt( const QskSpinBox* spinBox, const QPointF& pos )
    {
        if ( spinBox->subControlRect( QskSpinBox::IncrementPanel ).contains( pos ) )
            return ButtonIncrement;

        if ( spinBox->subControlRect( QskSpinBox::DecrementPanel ).contains( pos ) )
            return ButtonDecrement;

        return ButtonNone;
    }
}

class QskSpinBox::PrivateData
{
  public:
    inline void setPressed( QskSpinBox* spinBox, int button )
    {
        if ( pressedButton != button )
        {
            pressedButton = button;
            spinBox->update();
        }
    }

    inline void setHovered( QskSpinBox* spinBox, int button )
    {
        if ( hoveredButton != button )
        {
            hoveredButton = button;
            spinBox->update();
        }
    }

    int pressedButton = ButtonNone;
    int hoveredButton = ButtonNone;

    // int decimals; ???
};

QskSpinBox::QskSpinBox( QQuickItem* const parent )
    : Inherited( parent )
    , m_data( new PrivateData )
{
    setBoundaries( 0.0, 99.99 ); // this is what QDoubleSpinBox does

    setAcceptHoverEvents( true );
    setAcceptedMouseButtons( Qt::LeftButton );
    setFocusPolicy( Qt::StrongFocus );
}

QskSpinBox::~QskSpinBox()
{
}

void QskSpinBox::hoverEnterEvent( QHoverEvent* )
{
}

void QskSpinBox::hoverLeaveEvent( QHoverEvent* )
{
    m_data->setHovered( this, ButtonNone );
}

void QskSpinBox::hoverMoveEvent( QHoverEvent* event )
{
    const auto button = buttonAt( this, qskHoverPosition( event ) );
    m_data->setHovered( this, button );
}

void QskSpinBox::mousePressEvent( QMouseEvent* event )
{
    if ( const auto button = buttonAt( this, qskMousePosition( event ) ) )
    {
        m_data->setPressed( this, button );
        increment( stepSize() * button );

        return;
    }

    Inherited::mousePressEvent( event );
}

void QskSpinBox::mouseReleaseEvent( QMouseEvent* )
{
    m_data->setPressed( this, ButtonNone );
}

void QskSpinBox::keyPressEvent( QKeyEvent* const event )
{
    switch ( event->key() )
    {
        case Qt::Key_Minus:
        case Qt::Key_Down:
        case Qt::Key_Left:
        {
            increment( -stepSize() );
            m_data->setPressed( this, ButtonDecrement );
            return;
        }

        case Qt::Key_Plus:
        case Qt::Key_Up:
        case Qt::Key_Right:
        {
            increment( +stepSize() );
            m_data->setPressed( this, ButtonIncrement );
            return;
        }

#if 1
        case Qt::Key_Select:
        case Qt::Key_Space:

            /*
                All keys to navigate along the focus tab chain are not valid
                for a spin box, as it accepts number inputs only. Guess this is why
                QSpinBox goes straight into edit mode when receiving the focus

                So once setting values by keyboard is implemented we have to decide
                how to do it here. TODO ...
             */
            return;
#endif

        default:
            break;
    }

    Inherited::keyPressEvent( event );
}

void QskSpinBox::keyReleaseEvent( QKeyEvent* const event )
{
    m_data->setPressed( this, ButtonNone );
    Inherited::keyReleaseEvent( event );
}

int QskSpinBox::pressedButton() const
{
    return m_data->pressedButton;
}

int QskSpinBox::hoveredButton() const
{
    return m_data->hoveredButton;
}
