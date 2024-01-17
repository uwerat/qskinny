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
    QskIntervalF qskFillInterval( const Q* ring )
    {
        qreal pos1, pos2;

        if ( ring->isIndeterminate() )
        {
            const auto pos = ring->positionHint( QskProgressIndicator::Fill );

            pos1 = pos2 = pos;
        }
        else
        {
            pos1 = ring->valueAsRatio( ring->origin() );
            pos2 = ring->valueAsRatio( ring->value() );
        }

        if ( pos1 > pos2 )
            std::swap( pos1, pos2 );

        return QskIntervalF( pos1, pos2 );
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
    const auto ring = static_cast< const Q* >( skinnable );

    if( subControl == Q::Groove || subControl == Q::Fill )
    {
        auto rect = contentsRect;
        const auto size = ring->strutSizeHint( Q::Fill );

        if( ring->layoutMirroring() )
        {
            rect.setLeft( rect.right() - size.width() );
        }
        else
        {
            rect.setWidth( size.width() );
        }

        rect.setTop( rect.top() + 0.5 * ( rect.height() - size.height() ) );
        rect.setHeight( size.height() );

        return rect;
    }

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

    auto gradient = ring->gradientHint( subControl );
    if ( !gradient.isVisible() )
        return nullptr;

    if ( ( gradient.type() == QskGradient::Stops ) && !gradient.isMonochrome() )
    {
        const auto center = rect.center();
        const auto arcMetrics = ring->arcMetricsHint( Q::Fill );
        gradient.setConicDirection( center.x(), center.y(), arcMetrics.startAngle(), arcMetrics.spanAngle() );
    }

    const auto interval = qskFillInterval( ring );
    const auto arcMetrics = ring->arcMetricsHint( subControl );

    const auto startAngle = arcMetrics.startAngle() + interval.lowerBound() * arcMetrics.spanAngle();
    const auto spanAngle = interval.upperBound() * arcMetrics.spanAngle();

    return updateArcNode( ring, node, rect, gradient, startAngle, spanAngle, subControl );
}

QSizeF QskProgressRingSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto ring = static_cast< const Q* >( skinnable );

    const auto r = ring->strutSizeHint( Q::Fill );
    return r;
}

#include "moc_QskProgressRingSkinlet.cpp"
