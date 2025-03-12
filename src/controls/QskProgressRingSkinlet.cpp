/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskProgressRingSkinlet.h"
#include "QskArcHints.h"
#include "QskProgressRing.h"
#include "QskIntervalF.h"

using Q = QskProgressRing;

static QskIntervalF qskFillInterval( const QskProgressIndicator* indicator )
{
    qreal pos1, pos2;

    if ( indicator->isIndeterminate() )
    {
        pos1 = indicator->positionHint( QskProgressIndicator::Fill );
        pos2 = 2 * pos1;
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

static inline QPair< qreal, qreal > qskFillAngles(
    const QskArcMetrics& metrics, const QskIntervalF& intv )
{
    const auto startAngle = metrics.startAngle() + intv.lowerBound() * metrics.spanAngle();
    const auto endAngle = metrics.startAngle() + intv.upperBound() * metrics.spanAngle();

    return { startAngle, endAngle };
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

    const auto spacing = ring->spacingHint( Q::Fill ); // degrees

    if( spacing > 0.0 )
    {
        const auto fillMetrics = ring->arcMetricsHint( Q::Fill );
        if ( fillMetrics.isClosed() )
        {
            const auto fillAngles = qskFillAngles( fillMetrics, qskFillInterval( ring ) );

            qreal startAngle, endAngle;
            if ( fillAngles.second > fillAngles.first )
            {
                startAngle = fillAngles.second + spacing;
                endAngle = fillAngles.first + 360.0 - spacing;
            }
            else
            {
                startAngle = fillAngles.second - spacing;
                endAngle = fillAngles.first - 360.0 + spacing;
            }

            auto hints = indicator->arcHints( Q::Groove );
            hints.metrics.setStartAngle( startAngle );
            hints.metrics.setSpanAngle( endAngle - startAngle );

            const auto rect = indicator->subControlRect( Q::Groove );
            return updateArcNode( ring, node, rect, hints );
        }
    }

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

    auto hints = ring->arcHints( subControl );
    if ( !hints.isVisible() )
        return nullptr;

    const auto intv = qskFillInterval( ring );

    auto& gradient = hints.gradient;

    if ( ( gradient.type() == QskGradient::Stops ) && !gradient.isMonochrome() )
    {
        const auto stops = qskExtractedGradientStops( gradient.stops(),
            intv.lowerBound(), intv.upperBound() );

        gradient.setStops( stops );

        if ( hints.metrics.spanAngle() < 0.0 )
            gradient.reverse();
    }

    const auto angles = qskFillAngles( hints.metrics, intv );
    hints.metrics.setStartAngle( angles.first );
    hints.metrics.setSpanAngle( angles.second - angles.first );

    return updateArcNode( ring, node, rect, hints );
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
