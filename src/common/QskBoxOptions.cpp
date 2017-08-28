/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxOptions.h"

static inline bool qskIsVisible( const QColor& color )
{
    return color.isValid() && color.alpha() > 0;
}

QskBoxOptions::QskBoxOptions()
{
}

bool QskBoxOptions::isVisible() const
{
    if ( qskIsVisible( colors.fillColor( Qt::TopLeftCorner ) )
        || qskIsVisible( colors.fillColor( Qt::TopRightCorner ) )
        || qskIsVisible( colors.fillColor( Qt::BottomLeftCorner ) )
        || qskIsVisible( colors.fillColor( Qt::BottomRightCorner ) ) )
    {
        return true;
    }

    for ( auto edge : { Qt::LeftEdge, Qt::TopEdge, Qt::RightEdge, Qt::BottomEdge } )
    {
        if ( metrics.widthAt( edge ) && qskIsVisible( colors.borderColor( edge ) ) )
            return true;
    }

    return false;
}

QMarginsF QskBoxOptions::padding() const
{
    const QSizeF topLeft = metrics.radius( Qt::TopLeftCorner );
    const QSizeF topRight = metrics.radius( Qt::TopRightCorner );
    const QSizeF bottomLeft = metrics.radius( Qt::BottomLeftCorner );
    const QSizeF bottomRight = metrics.radius( Qt::BottomRightCorner );

    return QMarginsF( 
        std::max( topLeft.width(), bottomLeft.width() ),
        std::max( topLeft.height(), topRight.height() ),
        std::max( topRight.width(), bottomRight.width() ),
        std::max( bottomLeft.height(), bottomRight.height() ) );
}

QMarginsF QskBoxOptions::unitedMargins() const
{
    const QMarginsF pad = padding();

    // what the heck is this extra offset about - TODO ...
    const QMarginsF extra(
        std::max( 0.0, pad.left() - shadows.left() ) + 0.5,
        std::max( 0.0, pad.top() - shadows.top() ) + 0.5,
        std::max( 0.0, pad.right() - shadows.right() ) + 0.5,
        std::max( 0.0, pad.bottom() - shadows.bottom() ) + 0.5
    );

    return metrics.widths() + shadows + pad + extra;
}
