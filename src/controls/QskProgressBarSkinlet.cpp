/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskProgressBarSkinlet.h"
#include "QskProgressBar.h"
#include "QskIntervalF.h"
#include "QskBoxBorderMetrics.h"

#include <qeasingcurve.h>
#include <cmath>

static inline QskIntervalF qskBarInterval( const QskProgressBar* bar )
{
    qreal pos1, pos2;

    if ( bar->isIndeterminate() )
    {
        const auto pos = bar->metric( QskProgressBar::Bar | QskAspect::Position );

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

QskProgressBarSkinlet::QskProgressBarSkinlet( QskSkin* skin )
    : QskSkinlet( skin )
{
    setNodeRoles( { GrooveRole, BarRole } );
}

QskProgressBarSkinlet::~QskProgressBarSkinlet()
{
}

QRectF QskProgressBarSkinlet::subControlRect(
    const QskSkinnable* skinnable, const QRectF& contentsRect,
    QskAspect::Subcontrol subControl ) const
{
    const auto bar = static_cast< const QskProgressBar* >( skinnable );

    if( ( subControl == QskProgressBar::Groove ) )
    {
        const auto extent = bar->extent();

        auto rect = contentsRect;
        if ( bar->orientation() == Qt::Horizontal )
        {
            rect.setY( rect.y() + 0.5 * ( rect.height() - extent ) );
            rect.setHeight( extent );
        }
        else
        {
            rect.setX( rect.x() + 0.5 * ( rect.width() - extent ) );
            rect.setWidth( extent );
        }

        return rect;
    }

    if( subControl == QskProgressBar::Bar )
    {
        const auto bar = static_cast< const QskProgressBar* >( skinnable );
        return barRect( bar );
    }

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskProgressBarSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    switch( nodeRole )
    {
        case GrooveRole:
        {
            return updateBoxNode( skinnable, node, QskProgressBar::Groove );
        }

        case BarRole:
        {
            const auto bar = static_cast< const QskProgressBar* >( skinnable );
            return updateBarNode( bar, node );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSGNode* QskProgressBarSkinlet::updateBarNode(
    const QskProgressBar* bar, QSGNode* node ) const
{
    const auto subControl = QskProgressBar::Bar;

    const auto rect = bar->subControlRect( subControl );
    if ( rect.isEmpty() )
        return nullptr;

    auto gradient = bar->gradientHint( subControl );
    if ( !gradient.isVisible() )
        return nullptr;

    gradient.setOrientation( bar->orientation() );

    if ( !gradient.isMonochrome() )
    {
        const auto intv = qskBarInterval( bar );
        gradient = gradient.extracted( intv.lowerBound(), intv.upperBound() );

        if ( bar->orientation() == Qt::Vertical || bar->layoutMirroring() )
            gradient.reverse();
    }

    return updateBoxNode( bar, node, rect, gradient, subControl );
}

QRectF QskProgressBarSkinlet::barRect( const QskProgressBar* bar ) const
{
    const auto subControl = QskProgressBar::Groove;

    auto rect = bar->subControlRect( subControl );

    const auto borderMetrics = bar->boxBorderMetricsHint( subControl );

    auto m = bar->paddingHint( subControl );
    m += 0.5 * borderMetrics.toAbsolute( rect.size() ).widths();

    rect = rect.marginsRemoved( m );

    const auto intv = qskBarInterval( bar );

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

    const auto bar = static_cast< const QskProgressBar* >( skinnable );

    const auto extent = bar->extent();

    if ( bar->orientation() == Qt::Horizontal )
        return QSizeF( -1, extent );
    else
        return QSizeF( extent, -1 );
}

#include "moc_QskProgressBarSkinlet.cpp"
