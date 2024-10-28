/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskProgressRingSkinlet.h"
#include "QskArcMetrics.h"
#include "QskProgressRing.h"
#include "QskIntervalF.h"

using Q = QskProgressRing;

namespace
{
    inline bool qskIsContiguous( const QskProgressRing* ring )
    {
        return qFuzzyIsNull( ring->spacingHint( Q::Fill ) );
    }

    QskIntervalF fillInterval( const QskProgressIndicator* indicator )
    {
        qreal pos1, pos2;

        if ( indicator->isIndeterminate() )
        {
            pos1 = pos2 = indicator->positionHint( QskProgressIndicator::Fill );
        }
        else
        {
            pos1 = indicator->valueAsRatio( indicator->origin() );
            pos2 = indicator->valueAsRatio( indicator->value() );
        }

        if ( pos1 > pos2 )
            std::swap( pos1, pos2 );

        return QskIntervalF( pos1, pos2 );
    }

    inline QskArcMetrics fillAngles( const QskProgressRing* ring )
    {
        const auto intv = fillInterval( ring );

        const auto metrics = ring->arcMetricsHint( Q::Fill );

        if ( metrics.isNull() )
            return {};

        const auto startAngle = metrics.startAngle() + intv.lowerBound() * metrics.spanAngle();
        const auto spanAngle = intv.upperBound() * metrics.spanAngle();

        return QskArcMetrics( startAngle, spanAngle, 0 );
    }
}

QskProgressRingSkinlet::QskProgressRingSkinlet( QskSkin* skin )
    : Inherited( skin )
{
}

QskProgressRingSkinlet::~QskProgressRingSkinlet()
{
}

QRectF QskProgressRingSkinlet::subControlRect(
    const QskSkinnable* skinnable, const QRectF& contentsRect,
    QskAspect::Subcontrol subControl ) const
{
    if( subControl == Q::Groove || subControl == Q::Fill )
        return contentsRect;

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskProgressRingSkinlet::updateGrooveNode(
    const QskProgressIndicator* indicator, QSGNode* node ) const
{
    const auto ring = static_cast< const Q* >( indicator );

    if( qskIsContiguous( ring ) )
    {
        return updateArcNode( indicator, node, Q::Groove );
    }
    else
    {
        const auto angles = fillAngles( ring );
        const auto spacing = ring->spacingHint( Q::Fill );
        const auto sign = ( angles.spanAngle() > 0 ) ? 1 : -1;
        const auto startAngle = angles.endAngle() + sign * spacing;
        const auto spanAngle = sign * 360 - angles.spanAngle() - sign * 2 * spacing;

        return updateArcNode( ring, node, startAngle, spanAngle, Q::Groove );
    }
}

QSGNode* QskProgressRingSkinlet::updateFillNode(
    const QskProgressIndicator* indicator, QSGNode* node ) const
{
    const auto ring = static_cast< const Q* >( indicator );

    const auto subControl = Q::Fill;

    const auto rect = ring->subControlRect( subControl );
    if ( rect.isEmpty() )
        return nullptr;

    const auto metrics = ring->arcMetricsHint( subControl );
    if ( metrics.isNull() )
        return nullptr;

    auto gradient = ring->gradientHint( subControl );
    if ( !gradient.isVisible() )
        return nullptr;

    const auto intv = fillInterval( ring );

    if ( ( gradient.type() == QskGradient::Stops ) && !gradient.isMonochrome() )
    {
        const auto stops = qskExtractedGradientStops( gradient.stops(),
            intv.lowerBound(), intv.upperBound() );

        gradient.setStops( stops );

        if ( metrics.spanAngle() < 0.0 )
            gradient.reverse();
    }

    const auto angles = fillAngles( ring );

    return updateArcNode( ring, node, rect, gradient, angles.startAngle(), angles.spanAngle(), subControl );
}

QSizeF QskProgressRingSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& constraint ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    auto hint = skinnable->strutSizeHint( Q::Fill );
    hint = hint.expandedTo( skinnable->strutSizeHint( Q::Groove ) );

    if ( !constraint.isEmpty() )
    {
        const qreal aspectRatio = hint.isEmpty() ? 1.0 : hint.width() / hint.height();

        if ( constraint.width() >= 0.0 )
            hint.setHeight( constraint.width() / aspectRatio );
        else 
            hint.setWidth( constraint.height() * aspectRatio );
    }

    return hint;
}

#include "moc_QskProgressRingSkinlet.cpp"
