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

class QQuickWindow;
class QSGGeometry;
class QRectF;

class QSK_EXPORT QskBoxRenderer
{
  public:
    QskBoxRenderer( const QQuickWindow* );
    ~QskBoxRenderer();

    /*
        Filling the geometry without any color information:
            see QSGGeometry::defaultAttributes_Point2D()

        - clip nodes
        - using shaders setting the color information
     */

    void setBorderLines( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&, QSGGeometry& );

    void setFillLines( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&, QSGGeometry& );

    void setFillLines( const QRectF&,
        const QskBoxShapeMetrics&, QSGGeometry& );

    /*
        Filling the geometry with color information:
            see QSGGeometry::defaultAttributes_ColoredPoint2D()

        Usually used in combination with QSGVertexColorMaterial
     */

    void setColoredBorderLines( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&,
        const QskBoxBorderColors&, QSGGeometry& );

    void setColoredFillLines( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&,
        const QskGradient&, QSGGeometry& );

    void setColoredBorderAndFillLines( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&,
        const QskBoxBorderColors&, const QskGradient&, QSGGeometry& );

    static bool isGradientSupported( const QskGradient& );
    static QskGradient effectiveGradient( const QskGradient& );

  private:
    // for adjustments to the target ( f.e devicePixelRatio )
    const QQuickWindow* m_window;
};

#endif
