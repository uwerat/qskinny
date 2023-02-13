/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPageIndicatorSkinlet.h"
#include "QskPageIndicator.h"

#include "QskSGNode.h"
#include "QskFunctions.h"

static QRectF qskBulletRect( const QskPageIndicator* indicator,
    const QRectF& rect, int index )
{
    using Q = QskPageIndicator;

    const auto n = indicator->count();
    if ( n <= 0 || index < 0 || index >= n )
        return QRectF();

    if ( indicator->layoutMirroring() )
    {
        if ( indicator->orientation() == Qt::Horizontal )
            index = n - ( index + 1 );
    }

    const auto size = indicator->strutSizeHint( Q::Bullet );
    const qreal spacing = indicator->spacingHint( Q::Panel );
    const auto alignment = indicator->alignmentHint( Q::Panel, Qt::AlignCenter );

    qreal x, y;

    if ( indicator->orientation() == Qt::Horizontal )
    {
        const auto maxWidth = n * size.width() + ( n - 1 ) * spacing;
        const auto r = qskAlignedRectF( rect, maxWidth, size.height(), alignment );

        x = r.x() + index * ( size.width() + spacing );
        y = r.y();
    }
    else
    {
        const auto maxHeight = n * size.height() + ( n - 1 ) * spacing;
        const auto r = qskAlignedRectF( rect, maxHeight, size.height(), alignment );

        x = r.x();
        y = r.y() + index * ( size.height() + spacing );;
    }

    return QRectF( x, y, size.width(), size.height() );
}

QskPageIndicatorSkinlet::QskPageIndicatorSkinlet( QskSkin* skin )
    : QskSkinlet( skin )
{
    setNodeRoles( { PanelRole, BulletsRole } );
}

QskPageIndicatorSkinlet::~QskPageIndicatorSkinlet()
{
}

QRectF QskPageIndicatorSkinlet::subControlRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl ) const
{
    if ( subControl == QskPageIndicator::Panel )
        return contentsRect;

    return Inherited::subControlRect( skinnable, contentsRect, subControl );
}

QSGNode* QskPageIndicatorSkinlet::updateSubNode(
    const QskSkinnable* skinnable, quint8 nodeRole, QSGNode* node ) const
{
    using Q = QskPageIndicator;

    switch ( nodeRole )
    {
        case PanelRole:
            return updateBoxNode( skinnable, node, Q::Panel );

        case BulletsRole:
            return updateSeriesNode( skinnable, Q::Bullet, node );
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

int QskPageIndicatorSkinlet::sampleCount(
    const QskSkinnable* skinnable, QskAspect::Subcontrol subControl ) const
{
    using Q = QskPageIndicator;

    if ( subControl == Q::Bullet )
    {
        const auto indicator = static_cast< const QskPageIndicator* >( skinnable );
        return indicator->count();
    }

    return Inherited::sampleCount( skinnable, subControl );
}

QRectF QskPageIndicatorSkinlet::sampleRect( const QskSkinnable* skinnable,
    const QRectF& contentsRect, QskAspect::Subcontrol subControl, int index ) const
{
    using Q = QskPageIndicator;

    if ( subControl == Q::Bullet )
    {
        const auto indicator = static_cast< const QskPageIndicator* >( skinnable );

        const auto rect = indicator->subControlContentsRect( Q::Panel );
        return qskBulletRect( indicator, rect, index );
    }

    return Inherited::sampleRect( skinnable, contentsRect, subControl, index );
}

int QskPageIndicatorSkinlet::sampleIndexAt(
    const QskSkinnable* skinnable, const QRectF& contentsRect,
    QskAspect::Subcontrol subControl, const QPointF& pos ) const
{
    // TODO ...
    return Inherited::sampleIndexAt( skinnable, contentsRect, subControl, pos );
}

QSGNode* QskPageIndicatorSkinlet::updateSampleNode( const QskSkinnable* skinnable,
    QskAspect::Subcontrol subControl, int index, QSGNode* node ) const
{
    using Q = QskPageIndicator;

    if ( subControl == Q::Bullet )
    {
        auto indicator = static_cast< const QskPageIndicator* >( skinnable );

        const auto rect = sampleRect( indicator, indicator->contentsRect(), Q::Bullet, index );
        const auto ratio = indicator->valueRatioAt( index );

        /*
            QskSkinnable::effectiveSkinHint() does not add the skinStates(), when
            the aspect already has a state. So we need add thmen here.
         */
        const auto selectedStates = Q::Selected | indicator->skinStates();

        return QskSkinlet::updateInterpolatedBoxNode( skinnable, node,
            rect, Q::Bullet, Q::Bullet | selectedStates, ratio );
    }

    return nullptr;
}

QSizeF QskPageIndicatorSkinlet::sizeHint( const QskSkinnable* skinnable,
    Qt::SizeHint which, const QSizeF& ) const
{
    using Q = QskPageIndicator;

    if ( which != Qt::PreferredSize )
        return QSizeF();

    const auto indicator = static_cast< const QskPageIndicator* >( skinnable );

    QSizeF size( 0.0, 0.0 );
    const int n = indicator->count();

    if ( n > 0 )
    {
        size = indicator->strutSizeHint( Q::Bullet );
        const qreal spacing = indicator->spacingHint( Q::Panel );

        if ( indicator->orientation() == Qt::Horizontal )
            size.rwidth() += ( n - 1 ) * ( size.width() + spacing );
        else
            size.rheight() += ( n - 1 ) * ( size.height() + spacing );
    }

    const auto hint = indicator->outerBoxSize( Q::Panel, size );
    return hint.expandedTo( indicator->strutSizeHint( Q::Panel ) );
}

#include "moc_QskPageIndicatorSkinlet.cpp"
