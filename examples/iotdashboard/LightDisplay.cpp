/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
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
QSK_SUBCONTROL( LightDisplay, Handle )

QSK_STATE( LightDisplay, Pressed, ( QskAspect::FirstUserState << 1 ) )

namespace
{
    bool angleInRange( const QskArcMetrics& metrics, qreal angle )
    {
        return angle >= metrics.startAngle() && angle <= metrics.endAngle();
    }

    qreal angleDiff( qreal angle1, qreal angle2 )
    {
        // Qt wraps around at 180 degrees
        qreal diff = qAbs( angle1 ) - qAbs( angle2 );
        return qAbs( diff );
    }
}

LightDisplay::LightDisplay( QQuickItem* parent )
    : QskBoundedValueInput( parent )
{
    setAlignmentHint( LeftLabel, Qt::AlignRight );
    setAlignmentHint( ValueText, Qt::AlignRight );

    setBoundaries( 0, 100 );
    setStepSize( 1.0 );
    setPageSteps( 10 );
}

bool LightDisplay::isPressed() const
{
    return hasSkinState( Pressed );
}

void LightDisplay::mousePressEvent( QMouseEvent* event )
{
    const auto handleRect = subControlRect( LightDisplay::Handle );
    if ( handleRect.contains( event->pos() ) )
    {
        setSkinStateFlag( Pressed );
        return;
    }

    Inherited::mousePressEvent( event );
}

void LightDisplay::mouseMoveEvent( QMouseEvent* event )
{
    if ( !isPressed() )
        return;

    const auto mousePos = qskMousePosition( event );
    const auto rect = subControlRect( ColdAndWarmArc );

    if( !arcContainsPoint( rect, mousePos ) )
    {
        setSkinStateFlag( Pressed, false );
        return;
    }

    const auto metrics = arcMetricsHint( ColdAndWarmArc );

    const int tolerance = 20;

    auto angle = angleFromPoint( rect, mousePos );
    if( !angleInRange( metrics, angle ) )
    {
        // we're slightly outside the range, but don't want to give up
        // the Pressed state

        if( angleDiff( angle, metrics.startAngle() ) < tolerance )
        {
            angle = metrics.startAngle();
        }
        else if( angleDiff( angle, metrics.endAngle() ) < tolerance )
        {
            angle = metrics.endAngle();
        }
    }

    const auto ratio = ( metrics.spanAngle() - angle ) / metrics.spanAngle();
    setValueAsRatio( ratio );
}

void LightDisplay::mouseReleaseEvent( QMouseEvent* /*event*/ )
{
    setSkinStateFlag( Pressed, false );
}

void LightDisplay::keyPressEvent( QKeyEvent* event )
{
    switch( event->key() )
    {
        case Qt::Key_Left:
            increment( -stepSize() );
            return;

        case Qt::Key_Right:
            increment( stepSize() );
            return;
    }

    Inherited::keyPressEvent( event );
}

qreal LightDisplay::angleFromPoint( const QRectF& rect, const QPointF& point ) const
{
    const QPointF circlePos( point.x() - rect.center().x(),
        rect.center().y() - point.y() );

    const qreal angle = qAtan2( circlePos.y(), circlePos.x() );
    return qRadiansToDegrees( angle );
}

bool LightDisplay::arcContainsPoint( const QRectF& rect, const QPointF& point ) const
{
    // putting this in an own function just because it might be useful
    // at other places in the future

    const auto metrics = arcMetricsHint( ColdAndWarmArc );
    const int tolerance = 20;

    // 1. check angle
    QPointF circlePos(
        point.x() - rect.center().x(),
        rect.center().y() - point.y() );

    const qreal angle = angleFromPoint( rect, point );

    const bool angleWithinRange = angleInRange( metrics, angle )
        || angleDiff( angle, metrics.startAngle() ) <= tolerance
        || angleDiff( angle, metrics.endAngle() ) <= tolerance;

    // 2. check whether point is on arc
    const qreal radiusMax = rect.width() / 2;
    const qreal arcWidth = metrics.thickness();
    const qreal radiusMin = radiusMax - arcWidth;

    const qreal polarRadius = qSqrt( qPow( circlePos.x(), 2 ) + qPow( circlePos.y(), 2 ) );
    const bool pointOnArc = ( polarRadius + tolerance ) > radiusMin
        && ( polarRadius - tolerance ) < radiusMax;

    return angleWithinRange && pointOnArc;
}

#include "moc_LightDisplay.cpp"
