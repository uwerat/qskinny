/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_ARC_RENDERER_H
#define QSK_ARC_RENDERER_H

#include "QskGlobal.h"

class QskArcMetrics;
class QskGradient;

class QSGGeometry;
class QRectF;
class QColor;

namespace QskArcRenderer
{
    /*
        Filling the geometry without any color information:
            see QSGGeometry::defaultAttributes_Point2D()

        - clip nodes
        - using shaders setting the color information
     */

    QSK_EXPORT void renderBorderGeometry( const QRectF&,
        const QskArcMetrics&, qreal borderWidth, QSGGeometry& );

    QSK_EXPORT void renderFillGeometry( const QRectF&,
        const QskArcMetrics&, qreal borderWidth, QSGGeometry& );

    QSK_EXPORT void renderFillGeometry( const QRectF&,
        const QskArcMetrics&, QSGGeometry& );

    /*
        Filling the geometry usually with color information:
            see QSGGeometry::defaultAttributes_ColoredPoint2D()
     */
    QSK_EXPORT bool isGradientSupported( const QskArcMetrics&, const QskGradient& );

    QSK_EXPORT void renderArc( const QRectF&,
        const QskArcMetrics&, qreal borderWidth, const QColor& borderColor,
        const QskGradient&, QSGGeometry& );

    QSK_EXPORT void renderArc( const QRectF&, const QskArcMetrics&,
        const QskGradient&, QSGGeometry& );
}

#endif
