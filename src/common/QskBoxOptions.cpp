/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxOptions.h"
#include <qhashfunctions.h>

QskBoxOptions::QskBoxOptions():
    radius( { 0, 0, 0, 0, 0, 0, 0, 0 } ),
    color( { 0, 0, 0, 0, 0, 0, 0, 0 } )
{
}

uint QskBoxOptions::metricsHash( uint seed ) const
{
    return qHashBits( this, 2 * sizeof( QMarginsF ) + 8 * sizeof( qreal ), seed );
}

uint QskBoxOptions::colorsHash( uint seed ) const
{
    return qHashBits( &color, 8 * sizeof( QRgb ), seed );
}

void QskBoxOptions::setBorder( qreal width )
{
    borders = QMarginsF( width, width, width, width );
}   

void QskBoxOptions::setBorder( qreal left, qreal top, qreal right, qreal bottom )
{
    borders = QMarginsF( left, top, right, bottom );
}

void QskBoxOptions::setRadius( qreal radius )
{
    this->radius = { radius, radius, radius, radius, radius, radius, radius, radius };
}       

void QskBoxOptions::setBorderRgb( QRgb rgb )
{
    color.borderLeft = color.borderTop =
        color.borderRight = color.borderBottom = rgb;
}

void QskBoxOptions::setBorderRgb( QRgb rgbLeft, QRgb rgbTop,
    QRgb rgbRight, QRgb rgbBottom )
{
    color.borderLeft = rgbLeft;
    color.borderTop = rgbTop;
    color.borderRight = rgbRight;
    color.borderBottom = rgbBottom;
}

void QskBoxOptions::setFillRgb( QRgb rgb )
{
    color.fillTopLeft = color.fillTopRight =
        color.fillBottomRight = color.fillBottomLeft = rgb;
}

void QskBoxOptions::setFillRgb( QRgb rgbTopLeft, QRgb rgbTopRight,
    QRgb rgbBottomRight, QRgb rgbBottomLeft )
{
    color.fillTopLeft = rgbTopLeft;
    color.fillTopRight = rgbTopRight;
    color.fillBottomRight = rgbBottomRight;
    color.fillBottomLeft = rgbBottomLeft;
}

bool QskBoxOptions::isVisible() const
{
    if ( qAlpha( color.fillTopLeft ) ||
        qAlpha( color.fillTopRight ) ||
        qAlpha( color.fillBottomRight ) ||
        qAlpha( color.fillBottomLeft ) )
    {
        return true;
    }

    if ( ( borders.left() && qAlpha( color.borderLeft ) ) ||
        ( borders.top() && qAlpha( color.borderTop ) ) ||
        ( borders.right() && qAlpha( color.borderRight ) ) ||
        ( borders.bottom() && qAlpha( color.borderBottom ) ) )
    {
        return true;
    }

    return false;
}

QMarginsF QskBoxOptions::padding() const
{
    const qreal left = std::max( radius.topLeftX, radius.bottomLeftX );
    const qreal top = std::max( radius.topLeftY, radius.topRightY );
    const qreal right = std::max( radius.topRightX, radius.bottomRightX );
    const qreal bottom = std::max( radius.bottomRightY, radius.bottomLeftY );

    return QMarginsF( std::max( 0.0, left ), std::max( 0.0, top ),
        std::max( 0.0, right ), std::max( 0.0, bottom ) );
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

    return borders + shadows + pad + extra;
}
