/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskProgressBarSkinlet.h"
#include "QskProgressBar.h"
#include "QskIntervalF.h"
#include "QskBoxBorderMetrics.h"

#include <qeasingcurve.h>
#include <cmath>

using Q = QskProgressBar;

namespace
{
    QRectF qskFullGrooveRect( const QskProgressBar* bar )
    {
        const auto grooveSize = bar->metric( Q::Groove | QskAspect::Size );

        auto rect = bar->contentsRect();

        if ( bar->orientation() == Qt::Horizontal )
        {
            rect.setY( rect.y() + 0.5 * ( rect.height() - grooveSize ) );
            rect.setHeight( grooveSize );
        }
        else
        {
            rect.setX( rect.x() + 0.5 * ( rect.width() - grooveSize ) );
            rect.setWidth( grooveSize );
        }

        return rect;
    }

    QskIntervalF qskFillInterval( const QskProgressIndicator* indicator )
    {
        qreal pos1, pos2;

        if ( indicator->isIndeterminate() )
        {
            const auto pos = indicator->positionHint( QskProgressIndicator::Fill );

            static const QEasingCurve curve( QEasingCurve::InOutCubic );

            const qreal off = 0.15;

            pos1 = curve.valueForProgress( qMax( pos - off, 0.0 ) );
            pos2 = curve.valueForProgress( qMin( pos + off, 1.0 ) );
        }
        else
        {
            pos1 = indicator->valueAsRatio( indicator->origin() );
            pos2 = indicator->valueAsRatio( indicator->value() );
        }

        auto bar = static_cast< const QskProgressBar* >( indicator );

        if( bar->orientation() == Qt::Horizontal )
        {
            if ( bar->layoutMirroring() )
            {
                pos1 = 1.0 - pos1;
                pos2 = 1.0 - pos2;
            }
        }

        if ( pos1 > pos2 )
            std::swap( pos1, pos2 );

        return QskIntervalF( pos1, pos2 );
    }

    inline bool qskIsContiguous( const QskProgressBar* bar )
    {
        return qFuzzyIsNull( bar->spacingHint( Q::Fill ) );
    }
}

QskProgressBarSkinlet::QskProgressBarSkinlet( QskSkin* skin )
    : Inherited( skin )
{
    setNodeRoles( { GrooveRole, FillRole, GrooveStopIndicatorRole } );
}

QskProgressBarSkinlet::~QskProgressBarSkinlet()
{
}

QRectF QskProgressBarSkinlet::subControlRect(
    const QskSkinnable* skinnable, const QRectF& contentsRect,
    QskAspect::Subcontrol subControl ) const
{
    const auto bar = static_cast< const Q* >( skinnable );

    if( subControl == Q::Fill )
    {
        return barRect( bar );
    }

    if( subControl == Q::GrooveStopIndicator )
    {
        return grooveStopIndicatorRect( bar );
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskProgressBarSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    const auto bar = static_cast< const Q* >( skinnable );

    switch( nodeRole )
    {
        case GrooveStopIndicatorRole:
        {
            if( bar->isIndeterminate() || bar->hasOrigin() )
            {
                return nullptr;
            }
            else
            {
                return updateBoxNode( skinnable, node, Q::GrooveStopIndicator );
            }
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSGNode* QskProgressBarSkinlet::updateGrooveNode(
    const QskProgressIndicator* indicator, QSGNode* node ) const
{
    return updateSeriesNode( indicator, Q::Groove, node );
}

QSGNode* QskProgressBarSkinlet::updateFillNode(
    const QskProgressIndicator* indicator, QSGNode* node ) const
{
    const auto bar = static_cast< const Q* >( indicator );

    const auto subControl = Q::Fill;

    const auto rect = indicator->subControlRect( subControl );
    if ( rect.isEmpty() )
        return nullptr;

    auto gradient = indicator->gradientHint( subControl );
    if ( !gradient.isVisible() )
        return nullptr;

    if ( ( gradient.type() == QskGradient::Stops ) && !gradient.isMonochrome() )
    {
        /*
            When having stops only we use a linear gradient,
            where the colors are increasing in direction of the
            progress value. We interprete the gradient as a
            definition for the 100% situation and have to adjust
            the stops for smaller bars.

            For this situation it would be more convenient to
            adjust the start/stop positions, but the box renderer is
            not supporting this yet. TODO ...
         */

        const auto intv = qskFillInterval( bar );

        const auto stops = qskExtractedGradientStops( gradient.stops(),
            intv.lowerBound(), intv.upperBound() );

        gradient.setStops( stops );

        gradient.setLinearDirection( static_cast< Qt::Orientation >( bar->orientation() ) );

        if ( bar->orientation() == Qt::Vertical || bar->layoutMirroring() )
            gradient.reverse();
    }

    return updateBoxNode( indicator, node, rect, gradient, subControl );
}

QSGNode* QskProgressBarSkinlet::updateSampleNode( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl, int index, QSGNode* node ) const
{
    const auto bar = static_cast< const QskProgressBar* >( skinnable );
    const auto rect = sampleRect( bar, bar->contentsRect(), subControl, index );

    return updateBoxNode( skinnable, node, rect, subControl );
}

QRectF QskProgressBarSkinlet::barRect( const Q* bar ) const
{
    const auto subControl = Q::Groove;

    const auto barSize = bar->metric( Q::Fill | QskAspect::Size );
    auto rect = qskFullGrooveRect( bar );

    if ( bar->orientation() == Qt::Horizontal )
    {
        rect.setY( rect.y() + 0.5 * ( rect.height() - barSize ) );
        rect.setHeight( barSize );
    }
    else
    {
        rect.setX( rect.x() + 0.5 * ( rect.width() - barSize ) );
        rect.setWidth( barSize );
    }

    const auto borderMetrics = bar->boxBorderMetricsHint( subControl );

    auto m = bar->paddingHint( subControl );
    m += 0.5 * borderMetrics.toAbsolute( rect.size() ).widths();

    rect = rect.marginsRemoved( m );

    const auto intv = qskFillInterval( bar );

    if( bar->orientation() == Qt::Horizontal )
    {
        const auto w = rect.width();

        rect.setRight( rect.left() + intv.upperBound() * w );
        rect.setLeft( rect.left() + intv.lowerBound() * w );
    }
    else
    {
        const auto h = rect.height();

        rect.setTop( rect.bottom() - h * intv.upperBound() );
        rect.setBottom( rect.bottom() - h * intv.lowerBound() );
    }

    return rect;
}

QSizeF QskProgressBarSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto bar = static_cast< const Q* >( skinnable );

    const auto extent = bar->extent();

    if ( bar->orientation() == Qt::Horizontal )
        return QSizeF( -1, extent );
    else
        return QSizeF( extent, -1 );
}

int QskProgressBarSkinlet::sampleCount( const QskSkinnable* skinnable, QskAspect::Subcontrol ) const
{
    const auto bar = static_cast< const Q* >( skinnable );
    const auto samples = ( ( bar->isIndeterminate() || bar->hasOrigin() ) && !qskIsContiguous( bar ) ) ? 2 : 1;
    return samples;
}

QRectF QskProgressBarSkinlet::sampleRect( const QskSkinnable* skinnable,
    const QRectF&, QskAspect::Subcontrol, int index ) const
{
    const auto bar = static_cast< const Q* >( skinnable );
    const auto br = barRect( bar );
    QRectF rect = qskFullGrooveRect( bar );
    const auto spacing = bar->spacingHint( Q::Fill );
    const auto isContiguous = qskIsContiguous( bar );

    if( isContiguous )
    {
        return rect;
    }
    else
    {
        if( bar->orientation() == Qt::Horizontal )
        {
            if( index == 0 )
            {
                rect.setLeft( br.right() + spacing );
            }
            else
            {
                rect.setRight( br.left() - spacing );
            }
        }
        else
        {
            if( index == 0 )
            {
                rect.setBottom( br.top() - spacing );
            }
            else
            {
                rect.setTop( br.bottom() + spacing );
            }
        }
    }

    return rect;
}

QRectF QskProgressBarSkinlet::grooveStopIndicatorRect( const QskProgressBar* bar ) const
{
    auto rect = qskFullGrooveRect( bar );
    const auto size = bar->strutSizeHint( Q::GrooveStopIndicator );

    if( bar->orientation() == Qt::Horizontal )
    {
        rect.setLeft( rect.right() - size.width() );
    }
    else
    {
        rect.setBottom( rect.top() + size.height() );
    }

    return rect;
}

#include "moc_QskProgressBarSkinlet.cpp"
