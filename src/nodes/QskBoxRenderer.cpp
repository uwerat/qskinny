/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxRenderer.h"
#include "QskRectRenderer.h"
#include "QskRoundedRectRenderer.h"
#include "QskBoxShapeMetrics.h"

#include "QskGradient.h"
#include "QskGradientDirection.h"

void QskBoxRenderer::renderBorder(
    const QRectF& rect, const QskBoxShapeMetrics& shape,
    const QskBoxBorderMetrics& border, QSGGeometry& geometry )
{
    if ( shape.isRectangle() )
        QskRectRenderer::renderBorder( rect, border, geometry );
    else
        QskRoundedRectRenderer::renderRectellipseBorder( rect, shape, border, geometry );
}

void QskBoxRenderer::renderFill(
    const QRectF& rect, const QskBoxShapeMetrics& shape,
    const QskBoxBorderMetrics& border, QSGGeometry& geometry )
{
    if ( shape.isRectangle() )
        QskRectRenderer::renderFill( rect, border, geometry );
    else
        QskRoundedRectRenderer::renderRectellipseFill( rect, shape, border, geometry );
}

void QskBoxRenderer::renderBox( const QRectF& rect,
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& border,
    const QskBoxBorderColors& borderColors, const QskGradient& gradient,
    QSGGeometry& geometry )
{
    if ( shape.isRectangle() )
    {
        QskRectRenderer::renderRect(
            rect, border, borderColors, gradient, geometry );
    }
    else
    {
        QskRoundedRectRenderer::renderRectellipse(
            rect, shape, border, borderColors, gradient, geometry );
    }
}

bool QskBoxRenderer::isGradientSupported(
    const QskBoxShapeMetrics& shape, const QskGradient& gradient )
{
    if ( !gradient.isVisible() || gradient.isMonochrome() )
        return true;

    if ( gradient.spreadMode() != QskGradient::PadSpread )
    {
        // Only true if the situation requires spreading TODO ...
        return false;
    }

    switch( gradient.type() )
    {
        case QskGradient::Stops:
        {
            // will be rendered as vertical linear gradient
            return true;
        }
        case QskGradient::Linear:
        {
            const auto dir = gradient.linearDirection();
            if ( dir.x1() > dir.x2() || dir.y1() > dir.y2() )
                return false;

            if ( shape.isRectangle() )
            {
                // rectangles are fully supported
                return true;
            }

            if ( dir.isTilted() )
            {
                return ( dir.x1() == 0.0 && dir.x2() == 0.0 )
                    && ( dir.y1() == 1.0 && dir.y2() == 1.0 );
            }
            else
            {
                qreal r1, r2, r3, r4;

                if ( dir.isHorizontal() )
                {
                    r1 = shape.radius( Qt::TopLeftCorner ).width();
                    r2 = shape.radius( Qt::BottomLeftCorner ).width();
                    r3 = shape.radius( Qt::TopRightCorner ).width();
                    r4 = shape.radius( Qt::BottomRightCorner ).width();
                }
                else
                {
                    r1 = shape.radius( Qt::TopLeftCorner ).height();
                    r2 = shape.radius( Qt::TopRightCorner ).height();
                    r3 = shape.radius( Qt::BottomLeftCorner ).height();
                    r4 = shape.radius( Qt::BottomRightCorner ).height();
                }

                if ( ( r1 <= 0.0 || r2 <= 0.0 ) && ( r3 <= 0.0 || r4 <= 0.0 ) )
                {
                    // one of the corners is not rounded
                    return true;
                }

                // different radii at opposite corners are not implemented TODO ...
                return ( r1 == r2 ) && ( r3 == r4 );
            }

            return false;
        }

        default:
        {
            // Radial/Conical gradients have to be done with QskGradientMaterial
            return false;
        }
    }

    return false;
}
