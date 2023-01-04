/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOX_RENDERER_H
#define QSK_BOX_RENDERER_H

#include "QskGlobal.h"

class QskBoxBorderMetrics;
class QskBoxBorderColors;
class QskBoxShapeMetrics;
class QskGradient;

class QSGGeometry;
class QRectF;

namespace QskBoxRenderer
{
    QSK_EXPORT void renderBorder( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&, QSGGeometry& );

    QSK_EXPORT void renderFill( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&, QSGGeometry& );

    QSK_EXPORT void renderBox( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&,
        const QskBoxBorderColors&, const QskGradient&, QSGGeometry& );

    QSK_EXPORT bool isGradientSupported( const QskBoxShapeMetrics&, const QskGradient& );
}

#endif
