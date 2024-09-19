/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
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
    /*
        Filling the geometry without any color information:
            see QSGGeometry::defaultAttributes_Point2D()

        - clip nodes
        - using shaders setting the color information
     */

    QSK_EXPORT void setBorderLines( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&, QSGGeometry& );

    QSK_EXPORT void setFillLines( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&, QSGGeometry& );

    QSK_EXPORT void setFillLines( const QRectF&,
        const QskBoxShapeMetrics&, QSGGeometry& );

    /*
        Filling the geometry with color information:
            see QSGGeometry::defaultAttributes_ColoredPoint2D()

        Usually used in combination with QSGVertexColorMaterial
     */
    QSK_EXPORT bool isGradientSupported( const QskGradient& );

    QSK_EXPORT void setColoredBorderLines( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&,
        const QskBoxBorderColors&, QSGGeometry& );

    QSK_EXPORT void setColoredFillLines( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&,
        const QskGradient&, QSGGeometry& );

    QSK_EXPORT void setColoredBorderAndFillLines( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&,
        const QskBoxBorderColors&, const QskGradient&, QSGGeometry& );
}

#endif
