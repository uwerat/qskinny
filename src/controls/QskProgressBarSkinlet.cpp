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

static QskIntervalF qskFillInterval( const QskProgressIndicator* indicator )
{
    qreal pos1, pos2;

    if ( indicator->isIndeterminate() )
    {
        const auto pos = indicator->positionHint( Q::Fill );

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

static QskGradient qskFillGradient( const QskProgressBar* progressBar )
{
    auto gradient = progressBar->gradientHint( Q::Fill );

    if ( gradient.isVisible() && !gradient.isMonochrome()
        && ( gradient.type() == QskGradient::Stops ) )
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

        const auto intv = qskFillInterval( progressBar );

        const auto stops = qskExtractedGradientStops(
            gradient.stops(), intv.lowerBound(), intv.upperBound() );

        gradient.setStops( stops );

        gradient.setLinearDirection( progressBar->orientation() );

        if ( progressBar->orientation() == Qt::Vertical || progressBar->layoutMirroring() )
            gradient.reverse();
    }

    return gradient;
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
        return grooveRect( bar, contentsRect );

    if( subControl == Q::Fill )
        return fillRect( bar );

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
    const auto rect = indicator->subControlRect( Q::Fill );
    if ( rect.isEmpty() )
        return nullptr;

    const auto progressBar = static_cast< const Q* >( indicator );
    return updateBoxNode( indicator, node, rect,
        qskFillGradient( progressBar ), Q::Fill );
}

QRectF QskProgressBarSkinlet::grooveRect(
    const QskProgressBar* progressBar, const QRectF& contentsRect ) const
{
    const auto size = progressBar->metric( Q::Groove | QskAspect::Size );

    auto rect = contentsRect;
    if ( progressBar->orientation() == Qt::Horizontal )
    {
        rect.setY( rect.y() + 0.5 * ( rect.height() - size ) );
        rect.setHeight( size );
    }
    else
    {
        rect.setX( rect.x() + 0.5 * ( rect.width() - size ) );
        rect.setWidth( size );
    }

    return rect;
}

QRectF QskProgressBarSkinlet::fillRect( const QskProgressBar* progressBar ) const
{
    const auto size = progressBar->metric( Q::Fill | QskAspect::Size );
    auto rect = progressBar->subControlRect( Q::Groove );

    if ( progressBar->orientation() == Qt::Horizontal )
    {
        rect.setY( rect.y() + 0.5 * ( rect.height() - size ) );
        rect.setHeight( size );
    }
    else
    {
        rect.setX( rect.x() + 0.5 * ( rect.width() - size ) );
        rect.setWidth( size );
    }

    const auto borderMetrics = progressBar->boxBorderMetricsHint( Q::Groove );

    auto m = progressBar->paddingHint( Q::Groove );
    m += 0.5 * borderMetrics.toAbsolute( rect.size() ).widths();

    rect = rect.marginsRemoved( m );

    const auto intv = qskFillInterval( progressBar );

    if( progressBar->orientation() == Qt::Horizontal )
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

    auto extent = skinnable->metric( Q::Groove | QskAspect::Size );
    extent = qMax( extent, skinnable->metric( Q::Fill | QskAspect::Size ) );

    const auto progressBar = static_cast< const QskProgressBar* >( skinnable );

    if ( progressBar->orientation() == Qt::Horizontal )
        return QSizeF( -1, extent );
    else
        return QSizeF( extent, -1 );
}

#include "moc_QskProgressBarSkinlet.cpp"
