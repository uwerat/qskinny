/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_RECT_RENDERER_H
#define QSK_RECT_RENDERER_H

#include "QskGlobal.h"

class QskBoxBorderMetrics;
class QskBoxBorderColors;
class QskGradient;

class QSGGeometry;
class QRectF;

namespace QskRectRenderer
{
    void renderBorder( const QRectF&,
        const QskBoxBorderMetrics&, QSGGeometry& );

    void renderFill( const QRectF&,
        const QskBoxBorderMetrics&, QSGGeometry& );

    void renderRect( const QRectF&, const QskBoxBorderMetrics&,
        const QskBoxBorderColors&, const QskGradient&, QSGGeometry& );
}

#if 1

// maybe to QskVertex.h ???
namespace QskVertex
{
    class Quad;
    class ColoredLine;
}

namespace QskRectRenderer
{
    void renderFill0( const QskVertex::Quad&,
        const QskGradient&, int lineCount, QskVertex::ColoredLine* );
}

#endif

#endif
