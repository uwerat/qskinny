/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOX_RENDERER_H
#define QSK_BOX_RENDERER_H

#include "QskBoxShapeMetrics.h"
#include "QskVertex.h"
#include <qrect.h>

class QskBoxBorderMetrics;
class QskBoxBorderColors;
class QskGradient;

class QSGGeometry;

class QSK_EXPORT QskBoxRenderer
{
  public:
    static void renderBorder( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&, QSGGeometry& );

    static void renderFill( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&, QSGGeometry& );

    static void renderBox( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&,
        const QskBoxBorderColors&, const QskGradient&, QSGGeometry& );

    static bool isGradientSupported( const QskBoxShapeMetrics&, const QskGradient& );

    class Metrics
    {
      public:
        Metrics( const QRectF&, const QskBoxShapeMetrics&, const QskBoxBorderMetrics& );

        QskVertex::Quad outerQuad;
        QskVertex::Quad innerQuad;
#if 1
        QskVertex::Quad centerQuad; // to be removed
#endif

        struct Corner
        {
            bool isCropped;
            qreal centerX, centerY;
            qreal radiusX, radiusY;
            qreal radiusInnerX, radiusInnerY;

            int stepCount;

        } corner[ 4 ];

        bool isBorderRegular;
        bool isRadiusRegular;
        bool isTotallyCropped;
    };

  private:
    static void renderRectFill( const QRectF&,
        const QskBoxBorderMetrics&, QSGGeometry& );

    static void renderRectBorder( const QRectF&,
        const QskBoxBorderMetrics&, QSGGeometry& );

    static void renderRect( const QRectF&, const QskBoxBorderMetrics&,
        const QskBoxBorderColors&, const QskGradient&, QSGGeometry& );

    static void renderRectellipseFill( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&, QSGGeometry& );

    static void renderRectellipseBorder( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&, QSGGeometry& );

    static void renderRectellipse( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&,
        const QskBoxBorderColors&, const QskGradient&, QSGGeometry& );

    static void renderDiagonalFill( const Metrics&, const QskGradient&,
        int lineCount, QskVertex::ColoredLine* );

    static void renderRectFill( const QskVertex::Quad&,
        const QskGradient&, QskVertex::ColoredLine* );
};

inline void QskBoxRenderer::renderBorder(
    const QRectF& rect, const QskBoxShapeMetrics& shape,
    const QskBoxBorderMetrics& border, QSGGeometry& geometry )
{
    if ( shape.isRectangle() )
        renderRectBorder( rect, border, geometry );
    else
        renderRectellipseBorder( rect, shape, border, geometry );
}

inline void QskBoxRenderer::renderFill(
    const QRectF& rect, const QskBoxShapeMetrics& shape,
    const QskBoxBorderMetrics& border, QSGGeometry& geometry )
{
    if ( shape.isRectangle() )
        renderRectFill( rect, border, geometry );
    else
        renderRectellipseFill( rect, shape, border, geometry );
}

inline void QskBoxRenderer::renderBox( const QRectF& rect,
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& border,
    const QskBoxBorderColors& borderColors, const QskGradient& gradient,
    QSGGeometry& geometry )
{
    if ( shape.isRectangle() )
        renderRect( rect, border, borderColors, gradient, geometry );
    else
        renderRectellipse( rect, shape, border, borderColors, gradient, geometry );
}

#endif
