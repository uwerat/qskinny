/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskProgressBarSkinlet.h"
#include "QskProgressBar.h"

#include <cmath>

QskProgressBarSkinlet::QskProgressBarSkinlet( QskSkin* skin )
    : QskSkinlet( skin )
{
    setNodeRoles( { GrooveRole, ValueFillRole } );
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
        const auto dim = bar->thickness();
        
        auto rect = contentsRect;
        if ( bar->orientation() == Qt::Horizontal )
        {
            rect.setY( rect.y() + 0.5 * ( rect.height() - dim ) );
            rect.setHeight( dim );
        }
        else
        {
            rect.setX( rect.x() + 0.5 * ( rect.width() - dim ) );
            rect.setWidth( dim );
        }

        return rect;
    }

    if( subControl == QskProgressBar::ValueFill )
    {
        const auto bar = static_cast< const QskProgressBar* >( skinnable );
        return fillRect( bar );
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

        case ValueFillRole:
        {
            const auto bar = static_cast< const QskProgressBar* >( skinnable );
            return updateFillNode( bar, node );
        }
    }

    return Inherited::updateSubNode( skinnable, nodeRole, node );
}

QSGNode* QskProgressBarSkinlet::updateFillNode(
    const QskProgressBar* bar, QSGNode* node ) const
{
    const auto subControl = QskProgressBar::ValueFill;

    const auto rect = bar->subControlRect( subControl );
    if ( rect.isEmpty() )
        return nullptr;
    
    auto gradient = bar->gradientHint( subControl );
    if ( !gradient.isVisible() )
        return nullptr;

    gradient.setOrientation( bar->orientation() );

    if ( !gradient.isMonochrome() )
    {
        qreal pos1 = bar->valueAsRatio( bar->origin() );
        qreal pos2 = bar->valueAsRatio( bar->value() );

        if ( pos2 < pos1 )
            std::swap( pos1, pos2 );

        gradient = gradient.extracted( pos1, pos2 );

        if ( bar->orientation() == Qt::Vertical )
            gradient.reverse();
    }

    return updateBoxNode( bar, node, rect, gradient, subControl );
}

QRectF QskProgressBarSkinlet::fillRect( const QskProgressBar* bar ) const
{
    auto rect = bar->subControlRect( QskProgressBar::Groove );
    rect = bar->innerBox( QskProgressBar::Groove, rect );

    auto pos1 = bar->valueAsRatio( bar->origin() );
    auto pos2 = bar->valueAsRatio( bar->value() );

    if ( pos1 > pos2 )
        std::swap( pos1, pos2 );

    if( bar->orientation() == Qt::Horizontal )
    {
        const auto w = rect.width();

        rect.setRight( rect.left() + pos2 * w );
        rect.setLeft( rect.left() + pos1 * w );
    }
    else
    {
        const auto h = rect.height();

        rect.setTop( rect.bottom() - h * pos2 );
        rect.setBottom( rect.bottom() - h * pos1 );
    }

    return rect;
}

#include "moc_QskProgressBarSkinlet.cpp"
