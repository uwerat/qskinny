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

namespace QskRoundedRect { class Metrics; }

namespace QskRoundedRectRenderer
{
    void renderFillGeometry( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&, QSGGeometry& );

    void renderBorderGeometry( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&, QSGGeometry& );

    void renderRect( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&,
        const QskBoxBorderColors&, const QskGradient&, QSGGeometry& );

    // QskBoxRendererDEllipse.cpp
    void renderDiagonalFill( const QskRoundedRect::Metrics&,
        const QskGradient&, int lineCount, QskVertex::ColoredLine* );
}

#endif
