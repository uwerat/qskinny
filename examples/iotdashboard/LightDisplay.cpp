/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "LightDisplay.h"
#include "Skin.h"

#include <QskArcMetrics.h>
#include <QskEvent.h>

#include <QtMath>

QSK_SUBCONTROL( LightDisplay, Panel )
QSK_SUBCONTROL( LightDisplay, Groove )
QSK_SUBCONTROL( LightDisplay, ColdAndWarmArc )
QSK_SUBCONTROL( LightDisplay, Tickmarks )
QSK_SUBCONTROL( LightDisplay, ValueText )
QSK_SUBCONTROL( LightDisplay, LeftLabel )
QSK_SUBCONTROL( LightDisplay, RightLabel )
QSK_SUBCONTROL( LightDisplay, Knob )

QSK_STATE( LightDisplay, Pressed, ( QskAspect::FirstUserState << 1 ) )

LightDisplay::LightDisplay( QQuickItem* parent )
    : QskBoundedValueInput( parent )
{
    setAlignmentHint( LeftLabel, Qt::AlignRight );
    setAlignmentHint( ValueText, Qt::AlignRight );

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

    const auto mousePos = qskMousePosition( event );
    const auto rect = subControlRect( ColdAndWarmArc );

    bool arcContainsMousePos = arcContainsPoint( rect, mousePos );

    if( !arcContainsMousePos )
    {
        setSkinStateFlag( Pressed, false );
        return;
    }

    const QskArcMetrics metrics = arcMetricsHint( ColdAndWarmArc );
    const qreal angle = angleFromPoint( rect, mousePos );
    const qreal ratio = ( metrics.spanAngle() - angle * 16 ) / metrics.spanAngle();
    setValueAsRatio( ratio );
}

void LightDisplay::mouseReleaseEvent( QMouseEvent* /*event*/ )
{
    setSkinStateFlag( Pressed, false );
}

qreal LightDisplay::angleFromPoint( const QRectF& rect, const QPointF& point ) const
{
    QPointF circlePos( point.x() - rect.center().x(),
                 rect.center().y() - point.y() );

    const qreal atan = qAtan2( circlePos.y(), circlePos.x() );
    const qreal angle = qRadiansToDegrees( atan );
    return angle;
}

bool LightDisplay::arcContainsPoint( const QRectF& rect, const QPointF& point ) const
{
    // putting this in an own function just because it might be useful
    // at other places in the future

    const QskArcMetrics metrics = arcMetricsHint( ColdAndWarmArc );
    const int tolerance = 20;

    // 1. check angle
    QPointF circlePos( point.x() - rect.center().x(),
                 rect.center().y() - point.y() );

    const qreal angle = angleFromPoint( rect, point );
    const bool angleWithinRange = ( angle + tolerance ) > metrics.startAngle()
            && angle < ( metrics.startAngle() + metrics.spanAngle() - tolerance );

    // 2. check whether point is on arc
    const qreal radiusMax = rect.width() / 2;
    const qreal arcWidth = metrics.width();
    const qreal radiusMin = radiusMax - arcWidth;

    const qreal polarRadius = qSqrt( qPow( circlePos.x(), 2 ) + qPow( circlePos.y(), 2 ) );
    const bool pointOnArc = ( polarRadius + tolerance ) > radiusMin
            && ( polarRadius - tolerance ) < radiusMax;

    bool ret = angleWithinRange && pointOnArc;

    return ret;
}

#include "moc_LightDisplay.cpp"
