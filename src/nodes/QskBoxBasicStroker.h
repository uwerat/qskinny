/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOX_BASIC_STROKER_H
#define QSK_BOX_BASIC_STROKER_H

#include "QskBoxMetrics.h"
#include "QskBoxBorderColors.h"
#include "QskBoxColorMap.h"

class QskBoxShapeMetrics;
class QskBoxBorderMetrics;

namespace QskVertex
{
    class Line;
    class ColoredLine;
}

class QskBoxBasicStroker
{
  public:
    QskBoxBasicStroker( const QskBoxMetrics& );
    QskBoxBasicStroker( const QskBoxMetrics&, const QskBoxBorderColors& );
    QskBoxBasicStroker( const QskBoxMetrics&,
        const QskBoxBorderColors&, const QskBox::ColorMap& );

    int fillCount() const;
    int borderCount() const;

    /*
        QskVertex::Line ( = QSGGeometry::Point2D )

        Needed for:

            - monochrome coloring ( QSGSimpleMaterial )
            - clipping ( QSGClipNode )
            - shaders getting the color information from a color ramp
              ( = QskGradientMatrial )
     */

    void setBorderLines( QskVertex::Line* ) const;
    void setFillLines( QskVertex::Line* ) const;

    /*
        QskVertex::ColoredLine ( = QSGGeometry::ColoredPoint2D )

        The color information is added to the geometry what allows
        using the same shader regardless of the colors, what ends
        up in better scene graph batching
     */

    void setBorderLines( QskVertex::ColoredLine* ) const;
    void setFillLines( QskVertex::ColoredLine* ) const;

    void setBoxLines( QskVertex::ColoredLine*, QskVertex::ColoredLine* ) const;

  private:

    class GeometryLayout
    {
      public:
        GeometryLayout( const QskBoxMetrics&, const QskBoxBorderColors& );

        int cornerOffsets[ 4 ];
        int edgeOffsets[ 4 ];

        int closingOffsets[2];
        int lineCount;
    };

    void setBorderGradientLines( QskVertex::ColoredLine* ) const;
    void setBorderGradientLines( Qt::Edge, QskVertex::ColoredLine* ) const;

    void setBorderAndFillLines( QskVertex::ColoredLine*, QskVertex::ColoredLine* ) const;

    const QskBoxMetrics& m_metrics;
    const QskBoxBorderColors m_borderColors;
    const QskBox::ColorMap m_colorMap;
    const GeometryLayout m_geometryLayout;

    const bool m_isColored;
};

#endif
