/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
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
    QskIntervalF qskFillInterval( const Q* bar )
    {
        qreal pos1, pos2;

        if ( bar->isIndeterminate() )
        {
            const auto pos = bar->positionHint( QskProgressIndicator::Fill );

            static const QEasingCurve curve( QEasingCurve::InOutCubic );

            const qreal off = 0.15;

            pos1 = curve.valueForProgress( qMax( pos - off, 0.0 ) );
            pos2 = curve.valueForProgress( qMin( pos + off, 1.0 ) );
        }
        else
        {
            pos1 = bar->valueAsRatio( bar->origin() );
            pos2 = bar->valueAsRatio( bar->value() );
        }

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
}

QskProgressBarSkinlet::QskProgressBarSkinlet( QskSkin* skin )
    : Inherited( skin )
{
}

QskProgressBarSkinlet::~QskProgressBarSkinlet()
{
}

QRectF QskProgressBarSkinlet::subControlRect(
    const QskSkinnable* skinnable, const QRectF& contentsRect,
    QskAspect::Subcontrol subControl ) const
{
    const auto bar = static_cast< const Q* >( skinnable );

    if( subControl == Q::Groove )
    {
        const auto grooveSize = bar->metric( Q::Groove | QskAspect::Size );

        auto rect = contentsRect;
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

    if( subControl == Q::Fill )
    {
        return barRect( bar );
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskProgressBarSkinlet::updateGrooveNode(
    const QskProgressIndicator* indicator, QSGNode* node ) const
{
    return updateBoxNode( indicator, node, Q::Groove );
}

QSGNode* QskProgressBarSkinlet::updateFillNode(
    const QskProgressIndicator* indicator, QSGNode* node ) const
{
    const auto bar = static_cast< const Q* >( indicator );

    const auto subControl = Q::Fill;

    const auto rect = bar->subControlRect( subControl );
    if ( rect.isEmpty() )
        return nullptr;

    auto gradient = bar->gradientHint( subControl );
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

QRectF QskProgressBarSkinlet::barRect( const Q* bar ) const
{
    const auto subControl = Q::Groove;

    const auto barSize = bar->metric( Q::Fill | QskAspect::Size );
    auto rect = bar->subControlRect( subControl );

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

#include "moc_QskProgressBarSkinlet.cpp"
