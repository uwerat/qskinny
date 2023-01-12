/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskRoundedRect.h"
#include "QskGradient.h"
#include "QskGradientDirection.h"
#include "QskBoxShapeMetrics.h"
#include "QskBoxBorderColors.h"
#include "QskBoxBorderMetrics.h"

namespace
{
    Qt::Orientation orientationOf( const QskGradient& gradient )
    {
        if ( gradient.isMonochrome() )
            return Qt::Vertical;

        return gradient.linearDirection().isVertical()
            ? Qt::Vertical : Qt::Horizontal;
    }

    inline int extraStops( const QskGradient& gradient )
    {
        return qMax( 0, gradient.stepCount() - 1 );
    }

    inline void setBorderGradientLine(
        const QskVertex::Line& l, float dx1, float dy1, float dx2, float dy2,
        const QskGradientStop& stop, QskVertex::ColoredLine* lines )
    {
        const auto pos = stop.position();

        const float x1 = l.p1.x + pos * dx1;
        const float y1 = l.p1.y + pos * dy1;
        const float x2 = l.p2.x + pos * dx2;
        const float y2 = l.p2.y + pos * dy2;

        lines->setLine( x1, y1, x2, y2, stop.rgb() );
    }

    class ColorMap
    {
      public:
        inline ColorMap( const QskGradient& gradient )
            : m_isMonochrome( gradient.isMonochrome() )
            , m_color1( gradient.rgbStart() )
            , m_color2( gradient.rgbEnd() )
        {
        }

        inline QskVertex::Color colorAt( qreal value ) const
        {
            if ( m_isMonochrome )
                return m_color1;
            else
                return m_color1.interpolatedTo( m_color2, value );
        }

      private:
        const bool m_isMonochrome;
        const QskVertex::Color m_color1;
        const QskVertex::Color m_color2;
    };

    class BorderMap
    {
      public:
        inline BorderMap( int stepCount, const QskGradient& g1, const QskGradient& g2 )
            : m_stepCount( stepCount )
            , m_color1( g1.rgbStart() )
            , m_color2( g2.rgbEnd() )
        {
        }

        inline QskVertex::Color colorAt( int step ) const
        {
            if ( m_color1 == m_color2 )
                return m_color1;
            else
                return m_color1.interpolatedTo( m_color2, step / m_stepCount );
        }

      private:
        const qreal m_stepCount;
        const QskVertex::Color m_color1, m_color2;
    };

    class BorderMaps
    {
      public:
        inline BorderMaps( int stepCount, const QskBoxBorderColors& colors )
            : maps{
                { stepCount, colors.top(), colors.left() },
                { stepCount, colors.right(), colors.top() },
                { stepCount, colors.left(), colors.bottom() },
                { stepCount, colors.bottom(), colors.right() } }
        {
        }

        const BorderMap maps[4];
    };
}

int QskRoundedRect::extraBorderStops( const QskBoxBorderColors& bc )
{
    return extraStops( bc.left() ) + extraStops( bc.top() )
        + extraStops( bc.right() ) + extraStops( bc.bottom() );
}


QskRoundedRect::Metrics::Metrics( const QRectF& rect,
        const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& border )
    : outerQuad( rect )
{
    isRadiusRegular = shape.isRectellipse();

    for ( int i = 0; i < 4; i++ )
    {
        auto& c = corner[ i ];

        const QSizeF radius = shape.radius( static_cast< Qt::Corner >( i ) );
        c.radiusX = qBound( 0.0, radius.width(), 0.5 * outerQuad.width );
        c.radiusY = qBound( 0.0, radius.height(), 0.5 * outerQuad.height );
        c.stepCount = ArcIterator::segmentHint( qMax( c.radiusX, c.radiusY ) );

        switch ( i )
        {
            case TopLeft:
                c.centerX = outerQuad.left + c.radiusX;
                c.centerY = outerQuad.top + c.radiusY;
                break;

            case TopRight:
                c.centerX = outerQuad.right - c.radiusX;
                c.centerY = outerQuad.top + c.radiusY;
                break;

            case BottomLeft:
                c.centerX = outerQuad.left + c.radiusX;
                c.centerY = outerQuad.bottom - c.radiusY;
                break;

            case BottomRight:
                c.centerX = outerQuad.right - c.radiusX;
                c.centerY = outerQuad.bottom - c.radiusY;
                break;
        }
    }

    centerQuad.left = qMax( corner[ TopLeft ].centerX,
        corner[ BottomLeft ].centerX );

    centerQuad.right = qMin( corner[ TopRight ].centerX,
        corner[ BottomRight ].centerX );

    centerQuad.top = qMax( corner[ TopLeft ].centerY,
        corner[ TopRight ].centerY );

    centerQuad.bottom = qMin( corner[ BottomLeft ].centerY,
        corner[ BottomRight ].centerY );

    centerQuad.width = centerQuad.right - centerQuad.left;
    centerQuad.height = centerQuad.bottom - centerQuad.top;

    // now the bounding rectangle of the fill area

    const auto bw = border.widths();

    innerQuad.left = outerQuad.left + bw.left();
    innerQuad.right = outerQuad.right - bw.right();
    innerQuad.top = outerQuad.top + bw.top();
    innerQuad.bottom = outerQuad.bottom - bw.bottom();

    innerQuad.left = qMin( innerQuad.left, centerQuad.right );
    innerQuad.right = qMax( innerQuad.right, centerQuad.left );
    innerQuad.top = qMin( innerQuad.top, centerQuad.bottom );
    innerQuad.bottom = qMax( innerQuad.bottom, centerQuad.top );

    if ( innerQuad.left > innerQuad.right )
    {
        innerQuad.left = innerQuad.right =
            innerQuad.right + 0.5 * ( innerQuad.left - innerQuad.right );
    }

    if ( innerQuad.top > innerQuad.bottom )
    {
        innerQuad.top = innerQuad.bottom =
            innerQuad.bottom + 0.5 * ( innerQuad.top - innerQuad.bottom );
    }

    innerQuad.width = innerQuad.right - innerQuad.left;
    innerQuad.height = innerQuad.bottom - innerQuad.top;

    const qreal borderLeft = innerQuad.left - outerQuad.left;
    const qreal borderTop = innerQuad.top - outerQuad.top;
    const qreal borderRight = outerQuad.right - innerQuad.right;
    const qreal borderBottom = outerQuad.bottom - innerQuad.bottom;

    for ( int i = 0; i < 4; i++ )
    {
        auto& c = corner[ i ];

        switch ( i )
        {
            case TopLeft:
            {
                c.radiusInnerX = c.radiusX - borderLeft;
                c.radiusInnerY = c.radiusY - borderTop;

                c.isCropped = ( c.centerX <= innerQuad.left ) ||
                    ( c.centerY <= innerQuad.top );

                break;
            }
            case TopRight:
            {
                c.radiusInnerX = c.radiusX - borderRight;
                c.radiusInnerY = c.radiusY - borderTop;

                c.isCropped = ( c.centerX >= innerQuad.right ) ||
                    ( c.centerY <= innerQuad.top );
                break;
            }
            case BottomLeft:
            {
                c.radiusInnerX = c.radiusX - borderLeft;
                c.radiusInnerY = c.radiusY - borderBottom;

                c.isCropped = ( c.centerX <= innerQuad.left ) ||
                    ( c.centerY >= innerQuad.bottom );
                break;
            }
            case BottomRight:
            {
                c.radiusInnerX = c.radiusX - borderRight;
                c.radiusInnerY = c.radiusY - borderBottom;

                c.isCropped = ( c.centerX >= innerQuad.right ) ||
                    ( c.centerY >= innerQuad.bottom );
                break;
            }
        }
    }

    isTotallyCropped =
        corner[ TopLeft ].isCropped &&
        corner[ TopRight ].isCropped &&
        corner[ BottomRight ].isCropped &&
        corner[ BottomLeft ].isCropped;

    // number of steps for iterating over the corners

    isBorderRegular =
        ( borderLeft == borderTop ) &&
        ( borderTop == borderRight ) &&
        ( borderRight == borderBottom );
}

QskRoundedRect::BorderValues::BorderValues( const QskRoundedRect::Metrics& metrics )
    : m_metrics( metrics )
    , m_isUniform( metrics.isBorderRegular && metrics.isRadiusRegular )
{
    if ( m_isUniform )
    {
        const auto& c = metrics.corner[ 0 ];

        m_uniform.dx1 = c.radiusInnerX;
        m_uniform.dy1 = c.radiusInnerY;
    }
    else
    {
        for ( int i = 0; i < 4; i++ )
        {
            const auto& c = metrics.corner[ i ];

            auto& inner = m_multi.inner[ i ];
            auto& outer = m_multi.outer[ i ];

            if ( c.radiusInnerX >= 0.0 )
            {
                inner.x0 = 0.0;
                inner.rx = c.radiusInnerX;
            }
            else
            {
                // should also be c.isCropped !
                inner.x0 = c.radiusInnerX;
                inner.rx = 0.0;
            }

            if ( c.radiusInnerY >= 0.0 )
            {
                inner.y0 = 0.0;
                inner.ry = c.radiusInnerY;
            }
            else
            {
                // should also be c.isCropped !
                inner.y0 = c.radiusInnerY;
                inner.ry = 0.0;
            }

            outer.x0 = outer.y0 = 0.0;
            outer.rx = c.radiusX;
            outer.ry = c.radiusY;
        }
    }
}

void QskRoundedRect::BorderValues::setAngle( qreal cos, qreal sin )
{
    if ( m_isUniform )
    {
        const auto& c = m_metrics.corner[ 0 ];

        if ( !c.isCropped )
        {
            m_uniform.dx1 = cos * c.radiusInnerX;
            m_uniform.dy1 = sin * c.radiusInnerY;
        }

        m_uniform.dx2 = cos * c.radiusX;
        m_uniform.dy2 = sin * c.radiusY;
    }
    else
    {
        auto inner = m_multi.inner;
        auto outer = m_multi.outer;

        inner[ 0 ].setAngle( cos, sin );
        inner[ 1 ].setAngle( cos, sin );
        inner[ 2 ].setAngle( cos, sin );
        inner[ 3 ].setAngle( cos, sin );

        outer[ 0 ].setAngle( cos, sin );
        if ( !m_metrics.isRadiusRegular )
        {
            outer[ 1 ].setAngle( cos, sin );
            outer[ 2 ].setAngle( cos, sin );
            outer[ 3 ].setAngle( cos, sin );
        }
    }
}

void QskRoundedRect::Stroker::setBorderGradientLines(
    const BorderValues& v, int c1, const QskGradient& gradient,
    QskVertex::ColoredLine* lines ) const
{
    if( gradient.stepCount() <= 1 )
    {
        // everything done as contour lines
        return;
    }

    QskVertex::Line from, to;

    const auto& c = m_metrics.corner;

    switch( c1 )
    {
        case TopLeft:
        {
            const auto c2 = BottomLeft;

            to.p1.x = c[ c1 ].centerX - v.dx1( c1 );
            to.p1.y = c[ c1 ].centerY - v.dy1( c1 );
            to.p2.x = c[ c1 ].centerX - v.dx2( c1 );
            to.p2.y = c[ c1 ].centerY - v.dy2( c1 );

            from.p1.x = c[ c2 ].centerX - v.dx1( c2 );
            from.p1.y = c[ c2 ].centerY + v.dy1( c2 );
            from.p2.x = c[ c2 ].centerX - v.dx2( c2 );
            from.p2.y = c[ c2 ].centerY + v.dy2( c2 );

            break;
        }
        case TopRight:
        {
            const auto c2 = TopLeft;

            to.p1.x = c[ c1 ].centerX + v.dx1( c1 );
            to.p1.y = c[ c1 ].centerY - v.dy1( c1 );
            to.p2.x = c[ c1 ].centerX + v.dx2( c1 );
            to.p2.y = c[ c1 ].centerY - v.dy2( c1 );

            from.p1.x = c[ c2 ].centerX - v.dx1( c2 );
            from.p1.y = c[ c2 ].centerY - v.dy1( c2 );
            from.p2.x = c[ c2 ].centerX - v.dx2( c2 );
            from.p2.y = c[ c2 ].centerY - v.dy2( c2 );

            break;
        }
        case BottomLeft:
        {
            const auto c2 = BottomRight;

            to.p1.x = c[ c1 ].centerX - v.dx1( c1 );
            to.p1.y = c[ c1 ].centerY + v.dy1( c1 );
            to.p2.x = c[ c1 ].centerX - v.dx2( c1 );
            to.p2.y = c[ c1 ].centerY + v.dy2( c1 );

            from.p1.x = c[ c2 ].centerX + v.dx1( c2 );
            from.p1.y = c[ c2 ].centerY + v.dy1( c2 );
            from.p2.x = c[ c2 ].centerX + v.dx2( c2 );
            from.p2.y = c[ c2 ].centerY + v.dy2( c2 );

            break;
        }
        case BottomRight:
        {
            const auto c2 = TopRight;

            to.p1.x = c[ c1 ].centerX + v.dx1( c1 );
            to.p1.y = c[ c1 ].centerY + v.dy1( c1 );
            to.p2.x = c[ c1 ].centerX + v.dx2( c1 );
            to.p2.y = c[ c1 ].centerY + v.dy2( c1 );

            from.p1.x = c[ c2 ].centerX + v.dx1( c2 );
            from.p1.y = c[ c2 ].centerY - v.dy1( c2 );
            from.p2.x = c[ c2 ].centerX + v.dx2( c2 );
            from.p2.y = c[ c2 ].centerY - v.dy2( c2 );

            break;
        }
    }

    const float dx1 = to.p1.x - from.p1.x;
    const float dy1 = to.p1.y - from.p1.y;
    const float dx2 = to.p2.x - from.p2.x;
    const float dy2 = to.p2.y - from.p2.y;

    const auto stops = gradient.stops();

    auto line = lines;
    {
        const auto& stop = stops.last();

        if ( stop.position() < 1.0 )
            setBorderGradientLine( from, dx1, dy1, dx2, dy2, stop, line++ );
    }

    for( int i = stops.count() - 2; i >= 1; i-- )
        setBorderGradientLine( from, dx1, dy1, dx2, dy2, stops[i], line++ );

    {
        const auto& stop = stops.first();

        if ( stop.position() > 0.0 )
            setBorderGradientLine( from, dx1, dy1, dx2, dy2, stop, line++ );
    }
}

void QskRoundedRect::Stroker::createBorderLines( QskVertex::Line* lines ) const
{
    Q_ASSERT( m_metrics.isRadiusRegular );

    const auto& c = m_metrics.corner;

    const int stepCount = c[ 0 ].stepCount;
    const int numCornerLines = stepCount + 1;

    QskVertex::Line* linesBR = lines;
    QskVertex::Line* linesTR = linesBR + numCornerLines;
    QskVertex::Line* linesTL = linesTR + numCornerLines;
    QskVertex::Line* linesBL = linesTL + numCornerLines;

    BorderValues v( m_metrics );

    /*
        It would be possible to run over [0, 0.5 * M_PI_2]
        and create 8 values ( instead of 4 ) in each step. TODO ...
     */
    for ( ArcIterator it( stepCount, false ); !it.isDone(); ++it )
    {
        v.setAngle( it.cos(), it.sin() );

        const int j = it.step();
        const int k = numCornerLines - it.step() - 1;

        {
            constexpr auto corner = TopLeft;

            linesTL[ j ].setLine(
                c[ corner ].centerX - v.dx1( corner ),
                c[ corner ].centerY - v.dy1( corner ),
                c[ corner ].centerX - v.dx2( corner ),
                c[ corner ].centerY - v.dy2( corner ) );
        }

        {
            constexpr auto corner = TopRight;

            linesTR[ k ].setLine(
                c[ corner ].centerX + v.dx1( corner ),
                c[ corner ].centerY - v.dy1( corner ),
                c[ corner ].centerX + v.dx2( corner ),
                c[ corner ].centerY - v.dy2( corner ) );
        }

        {
            constexpr auto corner = BottomLeft;

            linesBL[ k ].setLine(
                c[ corner ].centerX - v.dx1( corner ),
                c[ corner ].centerY + v.dy1( corner ),
                c[ corner ].centerX - v.dx2( corner ),
                c[ corner ].centerY + v.dy2( corner ) );
        }

        {
            constexpr auto corner = BottomRight;

            linesBR[ j ].setLine(
                c[ corner ].centerX + v.dx1( corner ),
                c[ corner ].centerY + v.dy1( corner ),
                c[ corner ].centerX + v.dx2( corner ),
                c[ corner ].centerY + v.dy2( corner ) );
        }
    }

    lines[ 4 * numCornerLines ] = lines[ 0 ];
}

void QskRoundedRect::Stroker::createFillLines(
    QskVertex::ColoredLine* lines, const QskGradient& gradient ) const
{
    Q_ASSERT( m_metrics.isRadiusRegular );
    Q_ASSERT( gradient.isValid() && gradient.stepCount() <= 1 );

    ColorMap fillMap( gradient );

    const auto& c = m_metrics.corner;
    const int stepCount = c[ 0 ].stepCount;

    int numLines = 2 * stepCount + 1;

    const auto orientation = orientationOf( gradient );
    if ( orientation == Qt::Vertical )
    {
        if ( m_metrics.centerQuad.top < m_metrics.centerQuad.bottom )
            numLines++;
    }
    else
    {
        if ( m_metrics.centerQuad.left < m_metrics.centerQuad.right )
            numLines++;
    }

    BorderValues v( m_metrics );

    /*
        It would be possible to run over [0, 0.5 * M_PI_2]
        and create 8 values ( instead of 4 ) in each step. TODO ...
     */
    for ( ArcIterator it( stepCount, false ); !it.isDone(); ++it )
    {
        v.setAngle( it.cos(), it.sin() );

        const auto& ri = m_metrics.innerQuad;

        if ( orientation == Qt::Vertical )
        {
            const int j = it.step();
            const int k = numLines - it.step() - 1;

            const qreal x11 = c[ TopLeft ].centerX - v.dx1( TopLeft );
            const qreal x12 = c[ TopRight ].centerX + v.dx1( TopRight );
            const qreal y1 = c[ TopLeft ].centerY - v.dy1( TopLeft );
            const auto c1 = fillMap.colorAt( ( y1 - ri.top ) / ri.height );

            const qreal x21 = c[ BottomLeft ].centerX - v.dx1( BottomLeft );
            const qreal x22 = c[ BottomRight ].centerX + v.dx1( BottomRight );
            const qreal y2 = c[ BottomLeft ].centerY + v.dy1( BottomLeft );
            const auto c2 = fillMap.colorAt( ( y2 - ri.top ) / ri.height );

            lines[ j ].setLine( x11, y1, x12, y1, c1 );
            lines[ k ].setLine( x21, y2, x22, y2, c2 );
        }
        else
        {
            const int j = stepCount - it.step();
            const int k = numLines - 1 - stepCount + it.step();

            const qreal x1 = c[ TopLeft ].centerX - v.dx1( TopLeft );
            const qreal y11 = c[ TopLeft ].centerY - v.dy1( TopLeft );
            const qreal y12 = c[ BottomLeft ].centerY + v.dy1( BottomLeft );
            const auto c1 = fillMap.colorAt( ( x1 - ri.left ) / ri.width );

            const qreal x2 = c[ TopRight ].centerX + v.dx1( TopRight );
            const qreal y21 = c[ TopRight ].centerY - v.dy1( TopRight );
            const qreal y22 = c[ BottomRight ].centerY + v.dy1( BottomRight );
            const auto c2 = fillMap.colorAt( ( x2 - ri.left ) / ri.width );

            lines[ j ].setLine( x1, y11, x1, y12, c1 );
            lines[ k ].setLine( x2, y21, x2, y22, c2 );
        }
    }
}

void QskRoundedRect::Stroker::createBorderLines(
    Qt::Orientation orientation, QskVertex::ColoredLine* lines,
    const QskBoxBorderColors& colors ) const
{
    const auto& c = m_metrics.corner;
#if 1
    const int stepCount = c[ 0 ].stepCount;
#endif

    const BorderMaps borderMaps( stepCount, colors );

    QskVertex::ColoredLine* linesBR, * linesTR, * linesTL, * linesBL;

    const int numCornerLines = stepCount + 1;

    if ( orientation == Qt::Vertical )
    {
        linesBR = lines;
        linesTR = linesBR + numCornerLines + extraStops( colors.right() );
        linesTL = linesTR + numCornerLines + extraStops( colors.top() );
        linesBL = linesTL + numCornerLines + extraStops( colors.left() );
    }
    else
    {
        linesTR = lines + 1;
        linesTL = linesTR + numCornerLines + extraStops( colors.top() );
        linesBL = linesTL + numCornerLines + extraStops( colors.left() );
        linesBR = linesBL + numCornerLines + extraStops( colors.bottom() );
    }

    BorderValues v( m_metrics );

    /*
        It would be possible to run over [0, 0.5 * M_PI_2]
        and create 8 values ( instead of 4 ) in each step. TODO ...
     */
    for ( ArcIterator it( stepCount, false ); !it.isDone(); ++it )
    {
        v.setAngle( it.cos(), it.sin() );

        const int j = it.step();
        const int k = numCornerLines - it.step() - 1;

        {
            constexpr auto corner = TopLeft;

            linesTL[ j ].setLine(
                c[ corner ].centerX - v.dx1( corner ),
                c[ corner ].centerY - v.dy1( corner ),
                c[ corner ].centerX - v.dx2( corner ),
                c[ corner ].centerY - v.dy2( corner ),
                borderMaps.maps[ corner ].colorAt( j ) );
        }

        {
            constexpr auto corner = TopRight;

            linesTR[ k ].setLine(
                c[ corner ].centerX + v.dx1( corner ),
                c[ corner ].centerY - v.dy1( corner ),
                c[ corner ].centerX + v.dx2( corner ),
                c[ corner ].centerY - v.dy2( corner ),
                borderMaps.maps[ corner ].colorAt( k ) );
        }

        {
            constexpr auto corner = BottomLeft;

            linesBL[ k ].setLine(
                c[ corner ].centerX - v.dx1( corner ),
                c[ corner ].centerY + v.dy1( corner ),
                c[ corner ].centerX - v.dx2( corner ),
                c[ corner ].centerY + v.dy2( corner ),
                borderMaps.maps[ corner ].colorAt( k ) );
        }

        {
            constexpr auto corner = BottomRight;

            linesBR[ j ].setLine(
                c[ corner ].centerX + v.dx1( corner ),
                c[ corner ].centerY + v.dy1( corner ),
                c[ corner ].centerX + v.dx2( corner ),
                c[ corner ].centerY + v.dy2( corner ),
                borderMaps.maps[ corner ].colorAt( j ) );
        }
    }

    v.setAngle( 0.0, 1.0 );

    setBorderGradientLines( v, TopRight,
        colors.top(), linesTR + numCornerLines );

    setBorderGradientLines( v, BottomLeft,
        colors.bottom(), linesBL + numCornerLines );

    v.setAngle( 1.0, 0.0 );

    setBorderGradientLines( v, TopLeft,
        colors.left(), linesTL + numCornerLines );

    setBorderGradientLines( v, BottomRight,
        colors.right(), linesBR + numCornerLines );

    const int k = 4 * numCornerLines + extraBorderStops( colors );

    if ( orientation == Qt::Vertical )
        lines[ k ] = lines[ 0 ];
    else
        lines[ 0 ] = lines[ k ];
}

void QskRoundedRect::Stroker::createUniformBox(
    QskVertex::ColoredLine* borderLines, const QskBoxBorderColors& borderColors,
    QskVertex::ColoredLine* fillLines, const QskGradient& gradient ) const
{
    Q_ASSERT( m_metrics.isRadiusRegular );
    Q_ASSERT( gradient.isValid() && gradient.stepCount() <= 1 );

    const ColorMap fillMap( gradient );

    const auto& c = m_metrics.corner;
    const int stepCount = c[ 0 ].stepCount;

    const BorderMaps borderMaps( stepCount, borderColors );

    QskVertex::ColoredLine* linesBR, * linesTR, * linesTL, * linesBL;
    linesBR = linesTR = linesTL = linesBL = nullptr;

    const int numCornerLines = stepCount + 1;
    int numFillLines = fillLines ? 2 * numCornerLines : 0;

    const auto orientation = orientationOf( gradient );
    if ( orientation == Qt::Vertical )
    {
        if ( borderLines )
        {
            linesBR = borderLines;
            linesTR = linesBR + numCornerLines + extraStops( borderColors.right() );
            linesTL = linesTR + numCornerLines + extraStops( borderColors.top() );
            linesBL = linesTL + numCornerLines + extraStops( borderColors.left() );
        }

        if ( fillLines )
        {
            if ( m_metrics.centerQuad.top >= m_metrics.centerQuad.bottom )
                numFillLines--;
        }
    }
    else
    {
        if ( borderLines )
        {
            linesTR = borderLines + 1;
            linesTL = linesTR + numCornerLines + extraStops( borderColors.top() );
            linesBL = linesTL + numCornerLines + extraStops( borderColors.left() );
            linesBR = linesBL + numCornerLines + extraStops( borderColors.bottom() );
        }

        if ( fillLines )
        {
            if ( m_metrics.centerQuad.left >= m_metrics.centerQuad.right )
                numFillLines--;
        }
    }

    BorderValues v( m_metrics );

    /*
        It would be possible to run over [0, 0.5 * M_PI_2]
        and create 8 values ( instead of 4 ) in each step. TODO ...
     */
    for ( ArcIterator it( stepCount, false ); !it.isDone(); ++it )
    {
        v.setAngle( it.cos(), it.sin() );

        if ( borderLines )
        {
            const int j = it.step();
            const int k = numCornerLines - it.step() - 1;

            {
                constexpr auto corner = TopLeft;

                linesTL[ j ].setLine(
                    c[ corner ].centerX - v.dx1( corner ),
                    c[ corner ].centerY - v.dy1( corner ),
                    c[ corner ].centerX - v.dx2( corner ),
                    c[ corner ].centerY - v.dy2( corner ),
                    borderMaps.maps[ corner ].colorAt( j ) );
            }

            {
                constexpr auto corner = TopRight;

                linesTR[ k ].setLine(
                    c[ corner ].centerX + v.dx1( corner ),
                    c[ corner ].centerY - v.dy1( corner ),
                    c[ corner ].centerX + v.dx2( corner ),
                    c[ corner ].centerY - v.dy2( corner ),
                    borderMaps.maps[ corner ].colorAt( k ) );
            }

            {
                constexpr auto corner = BottomLeft;

                linesBL[ k ].setLine(
                    c[ corner ].centerX - v.dx1( corner ),
                    c[ corner ].centerY + v.dy1( corner ),
                    c[ corner ].centerX - v.dx2( corner ),
                    c[ corner ].centerY + v.dy2( corner ),
                    borderMaps.maps[ corner ].colorAt( k ) );
            }

            {
                constexpr auto corner = BottomRight;

                linesBR[ j ].setLine(
                    c[ corner ].centerX + v.dx1( corner ),
                    c[ corner ].centerY + v.dy1( corner ),
                    c[ corner ].centerX + v.dx2( corner ),
                    c[ corner ].centerY + v.dy2( corner ),
                    borderMaps.maps[ corner ].colorAt( j ) );
            }
        }

        if ( fillLines )
        {
            const auto& ri = m_metrics.innerQuad;

            if ( orientation == Qt::Vertical )
            {
                const int j = it.step();
                const int k = numFillLines - it.step() - 1;

                const qreal x11 = c[ TopLeft ].centerX - v.dx1( TopLeft );
                const qreal x12 = c[ TopRight ].centerX + v.dx1( TopRight );
                const qreal y1 = c[ TopLeft ].centerY - v.dy1( TopLeft );
                const auto c1 = fillMap.colorAt( ( y1 - ri.top ) / ri.height );

                const qreal x21 = c[ BottomLeft ].centerX - v.dx1( BottomLeft );
                const qreal x22 = c[ BottomRight ].centerX + v.dx1( BottomRight );
                const qreal y2 = c[ BottomLeft ].centerY + v.dy1( BottomLeft );
                const auto c2 = fillMap.colorAt( ( y2 - ri.top ) / ri.height );

                fillLines[ j ].setLine( x11, y1, x12, y1, c1 );
                fillLines[ k ].setLine( x21, y2, x22, y2, c2 );
            }
            else
            {
                const int j = stepCount - it.step();
                const int k = numFillLines - 1 - stepCount + it.step();

                const qreal x1 = c[ TopLeft ].centerX - v.dx1( TopLeft );
                const qreal y11 = c[ TopLeft ].centerY - v.dy1( TopLeft );
                const qreal y12 = c[ BottomLeft ].centerY + v.dy1( BottomLeft );
                const auto c1 = fillMap.colorAt( ( x1 - ri.left ) / ri.width );

                const qreal x2 = c[ TopRight ].centerX + v.dx1( TopRight );
                const qreal y21 = c[ TopRight ].centerY - v.dy1( TopRight );
                const qreal y22 = c[ BottomRight ].centerY + v.dy1( BottomRight );
                const auto c2 = fillMap.colorAt( ( x2 - ri.left ) / ri.width );

                fillLines[ j ].setLine( x1, y11, x1, y12, c1 );
                fillLines[ k ].setLine( x2, y21, x2, y22, c2 );
            }
        }
    }

    if ( borderLines )
    {
        v.setAngle( 0.0, 1.0 );

        setBorderGradientLines( v, TopRight,
            borderColors.top(), linesTR + numCornerLines );

        setBorderGradientLines( v, BottomLeft,
            borderColors.bottom(), linesBL + numCornerLines );

        v.setAngle( 1.0, 0.0 );

        setBorderGradientLines( v, TopLeft,
            borderColors.left(), linesTL + numCornerLines );

        setBorderGradientLines( v, BottomRight,
            borderColors.right(), linesBR + numCornerLines );

        const int k = 4 * numCornerLines + extraBorderStops( borderColors );

        if ( orientation == Qt::Vertical )
            borderLines[ k ] = borderLines[ 0 ];
        else
            borderLines[ 0 ] = borderLines[ k ];
    }
}
