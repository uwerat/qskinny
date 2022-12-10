/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxRenderer.h"
#include "QskBoxShapeMetrics.h"

#include "QskGradient.h"
#include "QskGradientDirection.h"

void QskBoxRenderer::renderBorder(
    const QRectF& rect, const QskBoxShapeMetrics& shape,
    const QskBoxBorderMetrics& border, QSGGeometry& geometry )
{
    if ( shape.isRectangle() )
        renderRectBorder( rect, border, geometry );
    else
        renderRectellipseBorder( rect, shape, border, geometry );
}

void QskBoxRenderer::renderFill(
    const QRectF& rect, const QskBoxShapeMetrics& shape,
    const QskBoxBorderMetrics& border, QSGGeometry& geometry )
{
    if ( shape.isRectangle() )
        renderRectFill( rect, border, geometry );
    else
        renderRectellipseFill( rect, shape, border, geometry );
}

void QskBoxRenderer::renderBox( const QRectF& rect,
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& border,
    const QskBoxBorderColors& borderColors, const QskGradient& gradient,
    QSGGeometry& geometry )
{
    if ( shape.isRectangle() )
        renderRect( rect, border, borderColors, gradient, geometry );
    else
        renderRectellipse( rect, shape, border, borderColors, gradient, geometry );
}

bool QskBoxRenderer::isGradientSupported(
    const QskBoxShapeMetrics&, const QskGradient& gradient )
{
    if ( !gradient.isVisible() || gradient.isMonochrome()
        || ( gradient.type() == QskGradient::Stops ) )
    {
        return true;
    }

    if ( gradient.type() == QskGradient::Linear )
    {
        const auto dir = gradient.linearDirection();

        if ( dir.isTilted() )
        {
            if ( dir.x1() == 0.0 && dir.y1() == 0.0
                && dir.x2() == 1.0 && dir.y2() == 1.0 )
            {
                return true;
            }
        }
        else
        {
            if ( dir.x1() == 0.0 && dir.x2() == 1.0 )
                return true;

            if ( dir.y1() == 0.0 && dir.y2() == 1.0 )
                return true;
        }
    }

    return false;
}
