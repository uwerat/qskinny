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
        - using shaders setting the colors
     */

    QSK_EXPORT void setBorderLines( const QRectF&,
        const QskArcMetrics&, bool radial, qreal borderWidth, QSGGeometry& );

    QSK_EXPORT void setFillLines( const QRectF&,
        const QskArcMetrics&, bool radial, qreal borderWidth, QSGGeometry& );

    /*
        Filling the geometry with color information:
            see QSGGeometry::defaultAttributes_ColoredPoint2D()

        Usually used in combination with QSGVertexColorMaterial
     */
    QSK_EXPORT bool isGradientSupported(
        const QRectF&, const QskArcMetrics&, const QskGradient& );

    QSK_EXPORT void setColoredBorderLines( const QRectF&,
        const QskArcMetrics&, bool radial, qreal borderWidth,
        const QColor& borderColor, QSGGeometry& );

    QSK_EXPORT void setColoredFillLines( const QRectF&,
        const QskArcMetrics&, bool radial, qreal borderWidth,
        const QskGradient&, QSGGeometry& );

    QSK_EXPORT void setColoredBorderAndFillLines( const QRectF&,
        const QskArcMetrics&, bool radial, qreal borderWidth,
        const QColor& borderColor, const QskGradient&, QSGGeometry& );
}

#endif
