/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOX_RENDERER_H
#define QSK_BOX_RENDERER_H

#include "QskVertex.h"

class QskBoxBorderMetrics;
class QskBoxBorderColors;
class QskBoxShapeMetrics;
class QskGradient;

class QSGGeometry;
class QRectF;

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

#endif
