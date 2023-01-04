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
            if ( shape.isRectangle() )
            {
                // rectangles are fully supported
                return true;
            }

            const auto dir = gradient.linearDirection();

            if ( dir.isTilted() )
            {
                return ( dir.x1() == 0.0 && dir.x2() == 0.0 )
                    && ( dir.y1() == 1.0 && dir.y2() == 1.0 );
            }
            else
            {
                // different radii at oppsoite corners are not implemented TODO ...

                const auto r1 = shape.radius( Qt::TopLeftCorner );
                const auto r2 = shape.radius( Qt::TopRightCorner );
                const auto r3 = shape.radius( Qt::BottomLeftCorner );
                const auto r4 = shape.radius( Qt::BottomRightCorner );

                if ( dir.isHorizontal() )
                {
                    if ( dir.x1() == 0.0 && dir.x2() == 1.0 )
                    {
                        return ( r1.width() == r3.width() )
                            && ( r2.width() == r4.width() );
                    }
                }
                else
                {
                    if ( dir.y1() == 0.0 && dir.y2() == 1.0 )
                    {
                        return ( r1.height() == r2.height() )
                            && ( r3.height() == r4.height() );
                    }
                }
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
