/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOX_METRICS_H
#define QSK_BOX_METRICS_H

#include <qrect.h>
#include <qglobal.h>
#include <qnamespace.h>

class QskBoxShapeMetrics;
class QskBoxBorderMetrics;

class QskBoxMetrics
{
  public:
    QskBoxMetrics( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics& );

    const QRectF outerRect;
    QRectF innerRect;

    int outerStepCount() const;
    int innerStepCount() const;

    int innerStepCount( int corner1, int corner2 ) const
    {
        return qMax( corners[ corner1 ].innerStepCount(),
            corners[ corner2 ].innerStepCount() );
    }

    struct Corner
    {
        inline qreal xInner( qreal cos ) const
            { return centerInnerX + sx * cos * radiusInnerX; }

        inline qreal yInner( qreal sin ) const
            { return centerInnerY + sy * sin * radiusInnerY; }

        inline qreal xOuter( qreal cos ) const
            { return centerX + sx * ( cos * radiusX ); }

        inline qreal yOuter( qreal sin ) const
            { return centerY + sy * ( sin * radiusY ); }

        inline int innerStepCount() const
            { return ( centerInnerX == 0.0 ) ? 1 : stepCount; }

        qreal centerX, centerY;
        qreal radiusX, radiusY;

        qreal centerInnerX, centerInnerY;
        qreal radiusInnerX, radiusInnerY;

        qreal sx, sy;

        int stepCount = 0;

    } corners[ 4 ];

    bool hasBorder = false;

    // the same border width on all sides
    bool isBorderRegular = false;

    // true for plain rectangles
    bool isOutsideRounded = false;
    /*
        outer radii are symmetric in both directions ( rectellipse )
        However the inner radii might differ when isBorderRegular is false
     */
    bool isOutsideSymmetric = false;

    /*
        When the border width exceed the inner radius the corner
        becomes rectangular at the inside. When this happens to
        all corners the inner part dgenerates to a rectangle and
        can be filled with a simpler algo.
     */
    bool isInsideRounded = false;

    /*
        stepSymmetries indicates the directions, where we can
        iterate with the the same steps along opposing corners.
        This is possible, when both corners have the same radius
        or one of them has a radius of 0.
     */
    Qt::Orientations stepSymmetries;

    /*
        In case stepSymmetries indicates both directions the direction
        that needs less steps is preferred.
     */
    Qt::Orientation preferredOrientation;
};

#endif
