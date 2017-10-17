/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOX_RENDERER_H
#define QSK_BOX_RENDERER_H

#include "QskGlobal.h"
#include "QskBoxShapeMetrics.h"

#include <QRectF>

class QskBoxBorderMetrics;
class QskBoxBorderColors;
class QskGradient;

class QSGGeometry;

namespace QskVertex
{
    class ColoredLine;
}

class QSK_EXPORT QskBoxRenderer
{
public:
    void renderBorder( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&, QSGGeometry& );

    void renderFill( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&, QSGGeometry& );

    void renderBox( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&,
        const QskBoxBorderColors&, const QskGradient&, QSGGeometry& );

    class Quad
    {
    public:
        inline constexpr Quad():
            left( 0.0 ),
            top( 0.0 ),
            right( 0.0 ),
            bottom( 0.0 ),
            width( 0.0 ),
            height( 0.0 )
        {
        }

        inline Quad( const QRectF& rect ):
            left( rect.left() ),
            top( rect.top() ),
            right( rect.right() ),
            bottom( rect.bottom() ),
            width( rect.width() ),
            height( rect.height() )
        {
        }

        inline bool operator==( const Quad& other ) const
        {
            return ( left == other.left ) && ( right == other.right )
                && ( top == other.top ) && ( bottom == other.bottom );
        }

        inline bool operator!=( const Quad& other ) const
        {
            return !( *this == other );
        }

        inline bool isEmpty() const
        {
            return ( width <= 0 ) || ( height <= 0 );
        }

        qreal left, top, right, bottom, width, height;
    };

    class Metrics
    {
    public:
        Metrics( const QRectF&, const QskBoxShapeMetrics&, const QskBoxBorderMetrics& );

        Quad outerQuad;
        Quad innerQuad;
#if 1
        Quad centerQuad; // to be removed
#endif

        struct Corner
        {
            bool isCropped;
            qreal centerX, centerY;
            qreal radiusX, radiusY;
            qreal radiusInnerX, radiusInnerY;

            int stepCount;

        } corner[4];

        bool isBorderRegular;
        bool isRadiusRegular;
        bool isTotallyCropped;
    };

private:
    void renderRectFill( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&, QSGGeometry& );

    void renderRectBorder( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&, QSGGeometry& );

    void renderRect( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&,
        const QskBoxBorderColors&, const QskGradient&, QSGGeometry& );

    void renderRectellipseFill( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&, QSGGeometry& );

    void renderRectellipseBorder( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&, QSGGeometry& );

    void renderRectellipse( const QRectF&,
        const QskBoxShapeMetrics&, const QskBoxBorderMetrics&,
        const QskBoxBorderColors&, const QskGradient&, QSGGeometry& );

    void renderDiagonalFill( const Metrics&, const QskGradient&,
        int lineCount, QskVertex::ColoredLine* );

    void renderRectFill( const Quad&, const QskGradient&, QskVertex::ColoredLine* );
};

inline void QskBoxRenderer::renderBorder(
    const QRectF& rect, const QskBoxShapeMetrics& shape,
    const QskBoxBorderMetrics& border, QSGGeometry& geometry )
{
    if ( shape.isRectangle() )
        renderRectBorder( rect, shape, border, geometry );
    else
        renderRectellipseBorder( rect, shape, border, geometry );
}

inline void QskBoxRenderer::renderFill(
    const QRectF& rect, const QskBoxShapeMetrics& shape,
    const QskBoxBorderMetrics& border, QSGGeometry& geometry )
{
    if ( shape.isRectangle() )
        renderRectFill( rect, shape, border, geometry );
    else
        renderRectellipseFill( rect, shape, border, geometry );
}

inline void QskBoxRenderer::renderBox( const QRectF& rect,
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& border,
    const QskBoxBorderColors& borderColors, const QskGradient& gradient,
    QSGGeometry& geometry )
{
    if ( shape.isRectangle() )
        renderRect( rect, shape, border, borderColors, gradient, geometry );
    else
        renderRectellipse( rect, shape, border, borderColors, gradient, geometry );
}

#endif
