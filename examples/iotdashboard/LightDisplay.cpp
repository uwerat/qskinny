/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "LightDisplay.h"
#include "Skin.h"

#include <QskEvent.h>

QSK_SUBCONTROL( LightDisplay, Panel )
QSK_SUBCONTROL( LightDisplay, Groove )
QSK_SUBCONTROL( LightDisplay, ColdAndWarmArc )
QSK_SUBCONTROL( LightDisplay, ValueText )
QSK_SUBCONTROL( LightDisplay, LeftLabel )
QSK_SUBCONTROL( LightDisplay, RightLabel )
QSK_SUBCONTROL( LightDisplay, Knob )

QSK_STATE( LightDisplay, Pressed, ( QskAspect::FirstUserState << 1 ) )

LightDisplay::LightDisplay( QQuickItem* parent )
    : QskBoundedValueInput( parent )
{
    setAlignmentHint( LeftLabel, Qt::AlignRight );
    setAlignmentHint( ValueText, Qt::AlignCenter | Qt::AlignHCenter );

    setBoundaries( 0, 100 );

    // ### move to Skin:
    setShadow( { 0, 20 } );
    setShadowColor( 0xe5e5e5 );
}

bool LightDisplay::isPressed() const
{
    return hasSkinState( Pressed );
}

void LightDisplay::setShadow( const QskShadowMetrics& shadow )
{
    m_shadow = shadow;
    update();
}

const QskShadowMetrics& LightDisplay::shadow() const
{
    return m_shadow;
}

void LightDisplay::setGradient( const QskGradient& gradient )
{
    m_gradient = gradient;
    update();
}

const QskGradient& LightDisplay::gradient() const
{
    return m_gradient;
}

void LightDisplay::setShadowColor( const QColor& color )
{
    m_shadowColor = color;
    update();
}

QColor LightDisplay::shadowColor() const
{
    return m_shadowColor;
}

void LightDisplay::mousePressEvent( QMouseEvent* event )
{
    QRectF handleRect = subControlRect( LightDisplay::Knob );

    if ( handleRect.contains( event->pos() ) )
    {
        setSkinStateFlag( Pressed );
    }
    else
    {
        QskBoundedValueInput::mousePressEvent( event );
    }
}

void LightDisplay::mouseMoveEvent( QMouseEvent* event )
{
    if ( !isPressed() )
        return;

    // ### check if arc contains the position

    const auto mousePos = qskMousePosition( event );
    const auto rect = subControlRect( ColdAndWarmArc );

    const qreal ratioX = ( mousePos.x() - rect.x() ) / rect.width();

    setValueAsRatio( ratioX );
}

void LightDisplay::mouseReleaseEvent( QMouseEvent* /*event*/ )
{
    setSkinStateFlag( Pressed, false );
}

#include "moc_LightDisplay.cpp"
