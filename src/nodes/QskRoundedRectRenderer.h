/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_ROUNDED_RECT_RENDERER_H
#define QSK_ROUNDED_RECT_RENDERER_H

#include "QskVertex.h"

class QskBoxBorderMetrics;
class QskBoxBorderColors;
class QskBoxShapeMetrics;
class QskGradient;

class QSGGeometry;
class QRectF;

namespace QskRoundedRectRenderer
{
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

    void renderRectellipseFill( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&, QSGGeometry& );

    void renderRectellipseBorder( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&, QSGGeometry& );

    void renderRectellipse( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&,
        const QskBoxBorderColors&, const QskGradient&, QSGGeometry& );

    void renderDiagonalFill( const Metrics&, const QskGradient&,
        int lineCount, QskVertex::ColoredLine* );
}

#endif
