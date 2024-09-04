/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskProgressRingSkinlet.h"
#include "QskArcMetrics.h"
#include "QskProgressRing.h"
#include "QskIntervalF.h"

using Q = QskProgressRing;

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
    return updateArcNode( indicator, node, Q::Groove );
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

    const auto startAngle = metrics.startAngle() + intv.lowerBound() * metrics.spanAngle();
    const auto spanAngle = intv.upperBound() * metrics.spanAngle();

    return updateArcNode( ring, node, rect, gradient, startAngle, spanAngle, subControl );
}

QSizeF QskProgressRingSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    return skinnable->strutSizeHint( Q::Fill );
}

QskIntervalF QskProgressRingSkinlet::fillInterval(
    const QskProgressIndicator* indicator ) const
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

#include "moc_QskProgressRingSkinlet.cpp"
