/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskRoundedRectRenderer.h"

#include "QskBoxBorderColors.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxRendererColorMap.h"
#include "QskBoxShapeMetrics.h"
#include "QskGradientDirection.h"
#include "QskRectRenderer.h"

#include <qmath.h>
#include <qsggeometry.h>

using namespace QskVertex;

namespace
{
    enum
    {
        TopLeft = Qt::TopLeftCorner,
        TopRight = Qt::TopRightCorner,
        BottomLeft = Qt::BottomLeftCorner,
        BottomRight = Qt::BottomRightCorner
    };

    int additionalGradientStops( const QskGradient& gradient )
    {
        return qMax( 0, gradient.stepCount() - 1 );
    }
}

namespace
{
    class BorderValuesUniform
    {
      public:
        inline BorderValuesUniform( const QskRoundedRectRenderer::Metrics& metrics )
            : m_corner( metrics.corner[ 0 ] )
            , m_dx1( m_corner.radiusInnerX )
            , m_dy1( m_corner.radiusInnerY )
        {
        }

        inline void setAngle( qreal cos, qreal sin )
        {
            if ( !m_corner.isCropped )
            {
                m_dx1 = cos * m_corner.radiusInnerX;
                m_dy1 = sin * m_corner.radiusInnerY;
            }

            m_dx2 = cos * m_corner.radiusX;
            m_dy2 = sin * m_corner.radiusY;
        }

        inline qreal dx1( int ) const { return m_dx1; }
        inline qreal dy1( int ) const { return m_dy1; }
        inline qreal dx2( int ) const { return m_dx2; }
        inline qreal dy2( int ) const { return m_dy2; }

      private:
        const QskRoundedRectRenderer::Metrics::Corner& m_corner;
        qreal m_dx1, m_dy1, m_dx2, m_dy2;
    };

    class BorderValuesMulti
    {
      public:
        inline BorderValuesMulti( const QskRoundedRectRenderer::Metrics& metrics )
            : m_uniform( metrics.isRadiusRegular )
        {
            for ( int i = 0; i < 4; i++ )
            {
                const auto& c = metrics.corner[ i ];
                auto& v = m_inner[ i ];

                if ( c.radiusInnerX >= 0.0 )
                {
                    v.x0 = 0.0;
                    v.rx = c.radiusInnerX;
                }
                else
                {
                    v.x0 = c.radiusInnerX;
                    v.rx = 0.0;
                }

                if ( c.radiusInnerY >= 0.0 )
                {
                    v.y0 = 0.0;
                    v.ry = c.radiusInnerY;
                }
                else
                {
                    v.y0 = c.radiusInnerY;
                    v.ry = 0.0;
                }

                m_outer[ i ].x0 = m_outer[ i ].y0 = 0.0;
                m_outer[ i ].rx = c.radiusX;
                m_outer[ i ].ry = c.radiusY;
            }
        }

        inline void setAngle( qreal cos, qreal sin )
        {
            m_inner[ 0 ].setAngle( cos, sin );
            m_inner[ 1 ].setAngle( cos, sin );
            m_inner[ 2 ].setAngle( cos, sin );
            m_inner[ 3 ].setAngle( cos, sin );

            m_outer[ 0 ].setAngle( cos, sin );
            if ( !m_uniform )
            {
                m_outer[ 1 ].setAngle( cos, sin );
                m_outer[ 2 ].setAngle( cos, sin );
                m_outer[ 3 ].setAngle( cos, sin );
            }
        }

        inline qreal dx1( int pos ) const { return m_inner[ pos].dx; }

        inline qreal dy1( int pos ) const { return m_inner[ pos ].dy; }

        inline qreal dx2( int pos ) const
            { return m_uniform ? m_outer[ 0 ].dx : m_outer[ pos ].dx; }

        inline qreal dy2( int pos ) const
            { return m_uniform ? m_outer[ 0 ].dy : m_outer[ pos ].dy; }

      private:
        const bool m_uniform;

        class Values
        {
          public:
            inline void setAngle( qreal cos, qreal sin )
            {
                dx = x0 + cos * rx;
                dy = y0 + sin * ry;
            }

            qreal dx, dy;
            qreal x0, y0, rx, ry;
        };

        Values m_inner[ 4 ];
        Values m_outer[ 4 ];
    };

    class CornerValues
    {
      public:
        inline void setCorner( Qt::Orientation orientation, bool increasing,
            const QskRoundedRectRenderer::Metrics::Corner& c )
        {
            if ( orientation == Qt::Horizontal )
            {
                m_center = c.centerX;
                m_radius = c.radiusInnerX;
            }
            else
            {
                m_center = c.centerY;
                m_radius = c.radiusInnerY;
            }

            const qreal f = increasing ? 1.0 : -1.0;

            if ( m_radius < 0.0 )
            {
                m_center += m_radius * f;
                m_radius = 0.0;
            }
            else
            {
                m_radius *= f;
            }

            stepCount = c.stepCount;
        }

        qreal valueAt( qreal fv ) const
        {
            return m_center + fv * m_radius;
        }

        int stepCount;

      private:
        qreal m_center = 0.0;
        qreal m_radius = 0.0;
    };
}

namespace
{
    class HVRectEllipseIterator
    {
      public:
        HVRectEllipseIterator(
                const QskRoundedRectRenderer::Metrics& metrics, const QLineF& vector )
            : m_metrics( metrics )
            , m_vertical( vector.x1() == vector.x2() )
        {
            const auto& mc = metrics.corner;

            auto v = m_values;

            if ( m_vertical )
            {
                const int c[] = { TopLeft, TopRight, BottomLeft, BottomRight };

                v[0].setCorner( Qt::Horizontal, false, mc[ c[0] ] );
                v[1].setCorner( Qt::Horizontal, true, mc[ c[1] ] );

                const int idx1 = ( v[0].stepCount >= v[1].stepCount ) ? 0 : 1;
                v[2].setCorner( Qt::Vertical, false, mc[ c[idx1] ] );

                v[3].setCorner( Qt::Horizontal, false, mc[ c[2] ] );
                v[4].setCorner( Qt::Horizontal, true, mc[ c[3] ] );

                const int idx2 = ( v[3].stepCount >= v[4].stepCount ) ? 2 : 3;
                v[5].setCorner( Qt::Vertical, true, mc[ c[idx2] ] );

                m_pos0 = metrics.innerQuad.top;
                m_size = metrics.innerQuad.height;

                m_t = m_pos0 + vector.y1() * m_size;
                m_dt = vector.dy() * m_size;
            }
            else
            {
                const int c[] = { TopLeft, BottomLeft, TopRight, BottomRight };

                v[0].setCorner( Qt::Vertical, false, mc[ c[0] ] );
                v[1].setCorner( Qt::Vertical, true, mc[ c[1] ] );

                const int idx1 = ( v[0].stepCount >= v[1].stepCount ) ? 0 : 1;
                v[2].setCorner( Qt::Horizontal, false, mc[ c[idx1] ] );

                v[3].setCorner( Qt::Vertical, false, mc[ c[2] ] );
                v[4].setCorner( Qt::Vertical, true, mc[ c[3] ] );

                const int idx2 = ( v[3].stepCount >= v[4].stepCount ) ? 2 : 3;
                v[5].setCorner( Qt::Horizontal, true, mc[ c[idx2] ] );

                m_pos0 = metrics.innerQuad.left;
                m_size = metrics.innerQuad.width;

                m_t = m_pos0 + vector.x1() * m_size;
                m_dt = vector.dx() * m_size;
            }

            m_v1.from = v[0].valueAt( 1.0 );
            m_v1.to = v[1].valueAt( 1.0 );
            m_v1.pos = m_pos0;

            m_v2 = m_v1;

            const auto stepCount = qMax( v[0].stepCount, v[1].stepCount );
            m_arcIterator.reset( stepCount, false );
        }

        inline bool advance()
        {
            auto v = m_values;

            if ( m_arcIterator.step() == m_arcIterator.stepCount() )
            {
                if ( m_arcIterator.isInverted() )
                {
                    // we have finished the closing "corners"
                    return false;
                }

                const auto stepCount = qMax( v[3].stepCount, v[4].stepCount );
                m_arcIterator.reset( stepCount, true );

                const qreal pos1 = v[2].valueAt( 0.0 );
                const qreal pos2 = v[5].valueAt( 0.0 );

                if ( pos1 < pos2 )
                {
                    // the real rectangle - between the rounded "corners "
                    m_v1 = m_v2;

                    m_v2.from = v[3].valueAt( 1.0 );
                    m_v2.to = v[4].valueAt( 1.0 );
                    m_v2.pos = pos2;

                    return true;
                }
            }

            m_arcIterator.increment();

            m_v1 = m_v2;

            if ( m_arcIterator.isInverted() )
                v += 3;

            m_v2.from = v[0].valueAt( m_arcIterator.cos() );
            m_v2.to = v[1].valueAt( m_arcIterator.cos() );
            m_v2.pos = v[2].valueAt( m_arcIterator.sin() );

            return true;
        }

        inline bool setGradientLine( qreal value, Color color, ColoredLine* line )
        {
            const auto pos = m_t + value * m_dt;

            const qreal f = ( pos - m_v1.pos ) / ( m_v2.pos - m_v1.pos );

            const qreal v1 = m_v1.from + f * ( m_v2.from - m_v1.from );
            const qreal v2 = m_v1.to + f * ( m_v2.to - m_v1.to );

            setLine( v1, v2, pos, color, line );
            return true;
        }

        inline void setContourLine( Color color, ColoredLine* line )
        {
            setLine( m_v2.from, m_v2.to, m_v2.pos, color, line );
        }

        inline qreal value() const
        {
            // coordinate translated into the gradiet vector
            return ( m_v2.pos - m_t ) / m_dt;
        }

      private:

        inline void setLine( qreal from, qreal to, qreal pos,
            Color color, ColoredLine* line )
        {
            if ( m_vertical )
                line->setLine( from, pos, to, pos, color );
            else
                line->setLine( pos, from, pos, to, color );
        }

        const QskRoundedRectRenderer::Metrics& m_metrics;
        const bool m_vertical;

        ArcIterator m_arcIterator;

        CornerValues m_values[6];
        struct { qreal from, to, pos; } m_v1, m_v2;

        qreal m_pos0, m_size;
        qreal m_t, m_dt; // to translate into gradient values
    };
}

namespace
{
    class BorderMapNone
    {
      public:
        static inline constexpr Color colorAt( int ) { return Color(); }
        inline QskGradient gradient() const { return QskGradient(); }
    };

    class BorderMapSolid
    {
      public:
        inline BorderMapSolid( const QskBoxBorderColors& colors )
            : m_color( colors.left().rgbStart() )
        {
        }

        inline Color colorAt( int ) const { return m_color; }
        inline QskGradient gradient() const { return QskGradient(); }

        const Color m_color;
    };

    class BorderMapGradient
    {
      public:
        inline BorderMapGradient( int stepCount,
                const QskGradient& gradient1, const QskGradient& gradient2 )
            : m_stepCount( stepCount )
            , m_color1( gradient1.rgbStart() )
            , m_color2( gradient2.rgbEnd() )
            , m_gradient( gradient2 )
        {
        }

        inline Color colorAt( int step ) const
        {
            return m_color1.interpolatedTo( m_color2, step / m_stepCount );
        }

        inline const QskGradient& gradient() const
        {
            return m_gradient;
        }

      private:
        const qreal m_stepCount;
        const Color m_color1, m_color2;
        const QskGradient m_gradient;
    };

    template< class Line, class BorderValues >
    class Stroker
    {
      public:
        inline Stroker( const QskRoundedRectRenderer::Metrics& metrics )
            : m_metrics( metrics )
        {
        }

        inline void setAdditionalLine(
            float x11, float y11, float x12, float y12,
            float dx1, float dy1, float dx2, float dy2,
            const QskGradientStop& stop, Line* line )
        {
            const auto pos = 1.0 - stop.position();

            const float x1 = x11 + pos * dx1;
            const float y1 = y11 + pos * dy1;
            const float x2 = x12 + pos * dx2;
            const float y2 = y12 + pos * dy2;

            line->setLine( x1, y1, x2, y2, stop.rgb() );
        }

        void addAdditionalLines(
            float x11, float y11, float x12, float y12, // start line
            float x21, float y21, float x22, float y22, // end line
            const QskGradient& gradient, Line* lines )
        {
            const float dx1 = x21 - x11;
            const float dy1 = y21 - y11;
            const float dx2 = x22 - x12;
            const float dy2 = y22 - y12;

            const auto stops = gradient.stops();

            auto line = lines + additionalGradientStops( gradient );

            {
                const auto& stop = stops.first();

                if ( stop.position() > 0.0 )
                    setAdditionalLine( x11, y11, x12, y12, dx1, dy1, dx2, dy2, stop, line-- );
            }

            for( int i = 1; i < stops.count() - 1; i++ )
            {
                setAdditionalLine( x11, y11, x12, y12, dx1, dy1, dx2, dy2, stops[i], line-- );
            }

            {
                const auto& stop = stops.last();

                if ( stop.position() < 1.0 )
                {
                    setAdditionalLine( x11, y11, x12, y12, dx1, dy1, dx2, dy2, stop, line-- );
                }
            }
        }

        template< class BorderMap, class FillMap >
        inline void createLines( Qt::Orientation orientation, Line* borderLines,
            const BorderMap& borderMapTL, const BorderMap& borderMapTR,
            const BorderMap& borderMapBL, const BorderMap& borderMapBR,
            Line* fillLines, FillMap& fillMap )
        {
            const auto& c = m_metrics.corner;
#if 1
            // TODO ...
            const int stepCount = c[ 0 ].stepCount;
#endif

            Line* linesBR, * linesTR, * linesTL, * linesBL;
            linesBR = linesTR = linesTL = linesBL = nullptr;

            const int numCornerLines = stepCount + 1;
            int numFillLines = fillLines ? 2 * numCornerLines : 0;

            if ( orientation == Qt::Vertical )
            {
                if ( borderLines )
                {
                    linesBR = borderLines;

                    linesTR = linesBR + numCornerLines
                        + additionalGradientStops( borderMapBR.gradient() );

                    linesTL = linesTR + numCornerLines
                        + additionalGradientStops( borderMapTR.gradient() );

                    linesBL = linesTL + numCornerLines
                        + additionalGradientStops( borderMapTL.gradient() );
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

                    linesTL = linesTR + numCornerLines
                        + additionalGradientStops( borderMapTR.gradient() );

                    linesBL = linesTL + numCornerLines
                        + additionalGradientStops( borderMapTL.gradient() );

                    linesBR = linesBL + numCornerLines
                        + additionalGradientStops( borderMapBL.gradient() );
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
                            borderMapTL.colorAt( j ) );
                    }

                    {
                        constexpr auto corner = TopRight;

                        linesTR[ k ].setLine(
                            c[ corner ].centerX + v.dx1( corner ),
                            c[ corner ].centerY - v.dy1( corner ),
                            c[ corner ].centerX + v.dx2( corner ),
                            c[ corner ].centerY - v.dy2( corner ),
                            borderMapTR.colorAt( k ) );
                    }

                    {
                        constexpr auto corner = BottomLeft;

                        linesBL[ k ].setLine(
                            c[ corner ].centerX - v.dx1( corner ),
                            c[ corner ].centerY + v.dy1( corner ),
                            c[ corner ].centerX - v.dx2( corner ),
                            c[ corner ].centerY + v.dy2( corner ),
                            borderMapBL.colorAt( k ) );
                    }

                    {
                        constexpr auto corner = BottomRight;

                        linesBR[ j ].setLine(
                            c[ corner ].centerX + v.dx1( corner ),
                            c[ corner ].centerY + v.dy1( corner ),
                            c[ corner ].centerX + v.dx2( corner ),
                            c[ corner ].centerY + v.dy2( corner ),
                            borderMapBR.colorAt( j ) );
                    }

                    // at the beginning and end of the loop we can add
                    // additional lines for border gradients:

                    if( j == 0 )
                    {
                        if( additionalGradientStops( borderMapTR.gradient() ) > 0 )
                        {
                            float x1TR = c[ TopRight ].centerX + v.dx1( TopRight ),
                                y1TR = c[ TopRight ].centerY - v.dy1( TopRight ),
                                x2TR = c[ TopRight ].centerX + v.dx2( TopRight ),
                                y2TR = c[ TopRight ].centerY - v.dy2( TopRight ),

                                x1TL = c[ TopLeft ].centerX - v.dx1( TopLeft ),
                                y1TL = c[ TopLeft ].centerY - v.dy1( TopLeft ),
                                x2TL = c[ TopLeft ].centerX - v.dx2( TopLeft ),
                                y2TL = c[ TopLeft ].centerY - v.dy2( TopLeft );

                            addAdditionalLines(
                                x1TR, y1TR, x2TR, y2TR,
                                x1TL, y1TL, x2TL, y2TL,
                                borderMapTR.gradient(), linesTR + k );
                        }

                        if( additionalGradientStops( borderMapBL.gradient() ) > 0 )
                        {
                            float x1BL = c[ BottomLeft ].centerX - v.dx1( BottomLeft ),
                                y1BL = c[ BottomLeft ].centerY + v.dy1( BottomLeft ),
                                x2BL = c[ BottomLeft ].centerX - v.dx2( BottomLeft ),
                                y2BL = c[ BottomLeft ].centerY + v.dy2( BottomLeft ),

                                x1BR = c[ BottomRight ].centerX + v.dx1( BottomRight ),
                                y1BR = c[ BottomRight ].centerY + v.dy1( BottomRight ),
                                x2BR = c[ BottomRight ].centerX + v.dx2( BottomRight ),
                                y2BR = c[ BottomRight ].centerY + v.dy2( BottomRight );

                            addAdditionalLines(
                                x1BL, y1BL, x2BL, y2BL,
                                x1BR, y1BR, x2BR, y2BR,
                                borderMapBL.gradient(), linesBL + k );
                        }
                    }

                    if( j == numCornerLines - 1 )
                    {
                        if( additionalGradientStops( borderMapTL.gradient() ) > 0 )
                        {
                            float x1TL = c[ TopLeft ].centerX - v.dx1( TopLeft ),
                                y1TL = c[ TopLeft ].centerY - v.dy1( TopLeft ),
                                x2TL = c[ TopLeft ].centerX - v.dx2( TopLeft ),
                                y2TL = c[ TopLeft ].centerY - v.dy2( TopLeft ),

                                x1BL = c[ BottomLeft ].centerX - v.dx1( BottomLeft ),
                                y1BL = c[ BottomLeft ].centerY + v.dy1( BottomLeft ),
                                x2BL = c[ BottomLeft ].centerX - v.dx2( BottomLeft ),
                                y2BL = c[ BottomLeft ].centerY + v.dy2( BottomLeft );

                            addAdditionalLines(
                                x1TL, y1TL, x2TL, y2TL,
                                x1BL, y1BL, x2BL, y2BL,
                                borderMapTL.gradient(), linesTL + j );
                        }

                        if( additionalGradientStops( borderMapBR.gradient() ) > 0 )
                        {
                            float x1BR = c[ BottomRight ].centerX + v.dx1( BottomRight ),
                                y1BR = c[ BottomRight ].centerY + v.dy1( BottomRight ),
                                x2BR = c[ BottomRight ].centerX + v.dx2( BottomRight ),
                                y2BR = c[ BottomRight ].centerY + v.dy2( BottomRight ),

                                x1TR = c[ TopRight ].centerX + v.dx1( TopRight ),
                                y1TR = c[ TopRight ].centerY - v.dy1( TopRight ),
                                x2TR = c[ TopRight ].centerX + v.dx2( TopRight ),
                                y2TR = c[ TopRight ].centerY - v.dy2( TopRight );

                            addAdditionalLines(
                                x1BR, y1BR, x2BR, y2BR,
                                x1TR, y1TR, x2TR, y2TR,
                                borderMapBR.gradient(), linesBR + j );
                        }
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

#if 1
            if ( borderLines )
            {
                const int additionalStops =
                    additionalGradientStops( borderMapBR.gradient() )
                    + additionalGradientStops( borderMapTR.gradient() )
                    + additionalGradientStops( borderMapTL.gradient() )
                    + additionalGradientStops( borderMapBL.gradient() );

                const int k = 4 * numCornerLines + additionalStops;

                if ( orientation == Qt::Vertical )
                    borderLines[ k ] = borderLines[ 0 ];
                else
                    borderLines[ 0 ] = borderLines[ k ];
            }
#endif
        }

      private:
        const QskRoundedRectRenderer::Metrics& m_metrics;
    };
}

static inline Qt::Orientation qskQtOrientation( const QskGradient& gradient )
{
    return gradient.linearDirection().isVertical() ? Qt::Vertical : Qt::Horizontal;
}

static inline int qskFillLineCount(
    const QskRoundedRectRenderer::Metrics& metrics, const QskGradient& gradient )
{
    if ( !gradient.isVisible() )
        return 0;

    int lineCount = 0;

    const auto dir = gradient.linearDirection();

    if ( dir.isVertical() )
    {
        lineCount += qMax( metrics.corner[ TopLeft ].stepCount,
            metrics.corner[ TopRight ].stepCount ) + 1;

        lineCount += qMax( metrics.corner[ BottomLeft ].stepCount,
            metrics.corner[ BottomRight ].stepCount ) + 1;

        if ( metrics.centerQuad.top >= metrics.centerQuad.bottom )
            lineCount--;
    }
    else if ( dir.isHorizontal() )
    {
        lineCount += qMax( metrics.corner[ TopLeft ].stepCount,
            metrics.corner[ BottomLeft ].stepCount ) + 1;

        lineCount += qMax( metrics.corner[ TopRight ].stepCount,
            metrics.corner[ BottomRight ].stepCount ) + 1;

        if ( metrics.centerQuad.left >= metrics.centerQuad.right )
            lineCount--;
    }
    else
    {
        lineCount += 2 * ( metrics.corner[ 0 ].stepCount + 1 );

        if ( metrics.centerQuad.left >= metrics.centerQuad.right )
            lineCount--;

        if ( metrics.centerQuad.top >= metrics.centerQuad.bottom )
            lineCount--;

        /*
            For diagonal lines the points at the opposite
            side are no points interpolating the outline.
            So we need to insert interpolating lines on both sides
         */

        lineCount *= 2; // a real ellipse could be done with lineCount lines: TODO ...

#if 1
        /*
            The termination of the fill algorithm is a bit random
            and might result in having an additional line.
            Until this effect is completely understood, we better
            reserve memory for this to avoid crashes.
         */

        lineCount++; // happens in a corner case - needs to be understood: TODO
#endif
    }

    // adding vertexes for the stops - beside the first/last

    if ( !gradient.isMonochrome() )
        lineCount += gradient.stepCount() - 1;

    return lineCount;
}

template< class Line, class BorderMap, class FillMap >
static inline void qskRenderLines(
    const QskRoundedRectRenderer::Metrics& metrics,
    Qt::Orientation orientation, Line* borderLines,
    const BorderMap& borderMapTL, const BorderMap& borderMapTR,
    const BorderMap& borderMapBL, const BorderMap& borderMapBR,
    Line* fillLines, const FillMap& fillMap )
{
    if ( metrics.isBorderRegular && metrics.isRadiusRegular )
    {
        // the same border width for all edges

        Stroker< Line, BorderValuesUniform > stroker( metrics );
        stroker.createLines( orientation, borderLines,
            borderMapTL, borderMapTR, borderMapBL, borderMapBR,
            fillLines, fillMap );
    }
    else
    {
        Stroker< Line, BorderValuesMulti > stroker( metrics );
        stroker.createLines( orientation, borderLines,
            borderMapTL, borderMapTR, borderMapBL, borderMapBR,
            fillLines, fillMap );
    }
}

template< class Line, class BorderMap, class FillMap >
static inline void qskRenderLines(
    const QskRoundedRectRenderer::Metrics& metrics, Qt::Orientation orientation,
    Line* borderLines, const BorderMap& borderMap, Line* fillLines,
    const FillMap& fillMap )
{
    qskRenderLines( metrics, orientation, borderLines,
        borderMap, borderMap, borderMap, borderMap, fillLines, fillMap );
}

template< class Line, class BorderMap >
static inline void qskRenderBorderLines(
    const QskRoundedRectRenderer::Metrics& metrics,
    Qt::Orientation orientation, Line* lines,
    const BorderMap& borderMapTL, const BorderMap& borderMapTR,
    const BorderMap& borderMapBL, const BorderMap& borderMapBR )
{
    qskRenderLines( metrics, orientation, lines, borderMapTL, borderMapTR,
        borderMapBL, borderMapBR, static_cast< Line* >( nullptr ), ColorMapNone() );
}

template< class Line, class BorderMap >
static inline void qskRenderBorderLines(
    const QskRoundedRectRenderer::Metrics& metrics,
    Qt::Orientation orientation, Line* lines, const BorderMap& borderMap )
{
    qskRenderBorderLines( metrics, orientation, lines,
        borderMap, borderMap, borderMap, borderMap );
}

template< class Line, class FillMap >
static inline void qskRenderFillLines( const QskRoundedRectRenderer::Metrics& metrics,
    Qt::Orientation orientation, Line* lines, const FillMap& fillMap )
{
    qskRenderLines( metrics, orientation,
        static_cast< Line* >( nullptr ), BorderMapNone(), lines, fillMap );
}

static inline void qskRenderBorder( const QskRoundedRectRenderer::Metrics& metrics,
    Qt::Orientation orientation, const QskBoxBorderColors& colors, ColoredLine* line )
{
    if ( colors.isMonochrome() )
    {
        qskRenderBorderLines( metrics, orientation,
            line, BorderMapSolid( colors ) );
    }
    else
    {
        const int stepCount = metrics.corner[ 0 ].stepCount;

        qskRenderBorderLines( metrics, orientation, line,
            BorderMapGradient( stepCount, colors.top(), colors.left() ),
            BorderMapGradient( stepCount, colors.right(), colors.top() ),
            BorderMapGradient( stepCount, colors.left(), colors.bottom() ),
            BorderMapGradient( stepCount, colors.bottom(), colors.right() ) );
    }
}

static inline void qskRenderFillRandom(
    const QskRoundedRectRenderer::Metrics& metrics,
    const QskGradient& gradient, ColoredLine* line )
{
    // here we know that: gradient.stepCount() <= 1 !

    const auto orientation = qskQtOrientation( gradient );

    if ( gradient.isMonochrome() )
    {
        const ColorMapSolid map( gradient );
        qskRenderFillLines( metrics, orientation, line, map );
    }
    else
    {
        const ColorMapGradient map( gradient );
        qskRenderFillLines( metrics, orientation, line, map );
    }
}

static inline void qskRenderBoxRandom(
    const QskRoundedRectRenderer::Metrics& metrics, const QskBoxBorderColors& borderColors,
    const QskGradient& gradient, ColoredLine* fillLine, ColoredLine* borderLine )
{
    // here we know that: gradient.stepCount() <= 1 !

    const auto& bc = borderColors;

    if ( bc.isMonochrome() )
    {
        const BorderMapSolid borderMap( bc );

        if ( gradient.isMonochrome() )
        {
            const ColorMapSolid fillMap( gradient );
            qskRenderLines( metrics, Qt::Vertical, borderLine, borderMap, fillLine, fillMap );
        }
        else
        {
            const auto orientation = qskQtOrientation( gradient );

            const ColorMapGradient fillMap( gradient );
            qskRenderLines( metrics, orientation, borderLine, borderMap, fillLine, fillMap );
        }
    }
    else
    {
        const int n = metrics.corner[ 0 ].stepCount;

        const auto& left = bc.left();
        const auto& top = bc.top();
        const auto& right = bc.right();
        const auto& bottom = bc.bottom();

        const BorderMapGradient tl( n, top, left );
        const BorderMapGradient tr( n, right, top );
        const BorderMapGradient bl( n, left, bottom );
        const BorderMapGradient br( n, bottom, right );

        if ( gradient.isMonochrome() )
        {
            const ColorMapSolid fillMap( gradient );
            qskRenderLines( metrics, Qt::Vertical, borderLine, tl, tr, bl, br, fillLine, fillMap );
        }
        else
        {
            const auto orientation = qskQtOrientation( gradient );

            const ColorMapGradient fillMap( gradient );
            qskRenderLines( metrics, orientation, borderLine, tl, tr, bl, br, fillLine, fillMap );
        }
    }
}

static inline void qskRenderFillOrdered(
    const QskRoundedRectRenderer::Metrics& metrics,
    const QskGradient& gradient, int lineCount, ColoredLine* lines )
{
    /*
        The algo for irregular radii at opposite corners is not yet
        implemented TODO ...
     */

    const auto dir = gradient.linearDirection();

    HVRectEllipseIterator it( metrics, dir.vector() );
    QskVertex::fillOrdered( it, gradient, lineCount,lines );
}

QskRoundedRectRenderer::Metrics::Metrics( const QRectF& rect,
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

void QskRoundedRectRenderer::renderRectellipseBorder(
    const QRectF& rect, const QskBoxShapeMetrics& shape,
    const QskBoxBorderMetrics& border, QSGGeometry& geometry )
{
    const Metrics metrics( rect, shape, border );

    if ( metrics.innerQuad == metrics.outerQuad )
    {
        allocateLines< Line >( geometry, 0 );
        return;
    }

    const int stepCount = metrics.corner[ 0 ].stepCount;
    const int lineCount = 4 * ( stepCount + 1 ) + 1;

    const auto line = allocateLines< Line >( geometry, lineCount );
    qskRenderBorderLines( metrics, Qt::Vertical, line, BorderMapNone() );
}

void QskRoundedRectRenderer::renderRectellipseFill(
    const QRectF& rect, const QskBoxShapeMetrics& shape,
    const QskBoxBorderMetrics& border, QSGGeometry& geometry )
{
    const Metrics metrics( rect, shape, border );

    if ( ( metrics.innerQuad.width <= 0 ) || ( metrics.innerQuad.height <= 0 ) )
    {
        geometry.allocate( 0 );
        return;
    }

    if ( metrics.isTotallyCropped )
    {
        // degenerated to a rectangle

        geometry.allocate( 4 );

        const auto& quad = metrics.innerQuad;

        auto p = geometry.vertexDataAsPoint2D();
        p[0].set( quad.left, quad.top );
        p[1].set( quad.right, quad.top );
        p[2].set( quad.left, quad.bottom );
        p[3].set( quad.right, quad.bottom );

        return;
    }

    /*
        Unfortunately QSGGeometry::DrawTriangleFan is no longer supported with
        Qt6 and we have to go with DrawTriangleStrip, duplicating the center with
        each each vertex.
     */

    const auto numPoints =
        metrics.corner[0].stepCount + metrics.corner[1].stepCount
        + metrics.corner[2].stepCount + metrics.corner[3].stepCount + 4;

    /*
        points: center point + interpolated corner points
        indexes: lines between the center and each point, where
                 the first line needs to be appended to close the filling
     */

    geometry.allocate( 1 + numPoints, 2 * ( numPoints + 1 ) );

    Q_ASSERT( geometry.sizeOfIndex() == 2 );

    auto p = geometry.vertexDataAsPoint2D();
    auto idx = geometry.indexDataAsUShort();

    int i = 0;

    p[i++].set( rect.x() + 0.5 * rect.width(), rect.y() + 0.5 * rect.height() );

    BorderValuesMulti v( metrics );

    {
        constexpr auto id = TopLeft;
        const auto& c = metrics.corner[ id ];

        for ( ArcIterator it( c.stepCount, false ); !it.isDone(); ++it )
        {
            *idx++ = 0;
            *idx++ = i;

            v.setAngle( it.cos(), it.sin() );
            p[i++].set( c.centerX - v.dx1( id ), c.centerY - v.dy1( id ) );
        }
    }
    {
        constexpr auto id = BottomLeft;
        const auto& c = metrics.corner[ id ];

        for ( ArcIterator it( c.stepCount, true ); !it.isDone(); ++it )
        {
            *idx++ = 0;
            *idx++ = i;

            v.setAngle( it.cos(), it.sin() );
            p[i++].set( c.centerX - v.dx1( id ), c.centerY + v.dy1( id ) );
        }
    }
    {
        constexpr auto id = BottomRight;
        const auto& c = metrics.corner[ id ];

        for ( ArcIterator it( c.stepCount, false ); !it.isDone(); ++it )
        {
            *idx++ = 0;
            *idx++ = i;

            v.setAngle( it.cos(), it.sin() );
            p[i++].set( c.centerX + v.dx1( id ), c.centerY + v.dy1( id ) );
        }
    }
    {
        constexpr auto id = TopRight;
        const auto& c = metrics.corner[ id ];

        for ( ArcIterator it( c.stepCount, true ); !it.isDone(); ++it )
        {
            *idx++ = 0;
            *idx++ = i;

            v.setAngle( it.cos(), it.sin() );
            p[i++].set( c.centerX + v.dx1( id ), c.centerY - v.dy1( id ) );
        }
    }

    *idx++ = 0;
    *idx++ = 1;

#if 0
    {
        auto p = geometry.vertexDataAsPoint2D();

        qDebug() << "Vertexes:" << geometry.vertexCount();
        for ( int i = 0; i < geometry.vertexCount(); i++ )
            qDebug() << "\t" << i << p[i].x << p[i].y;

        auto idx = geometry.indexDataAsUShort();

        qDebug() << "Indexes:" << geometry.indexCount();
        for ( int i = 0; i < geometry.indexCount(); i++ )
            qDebug() << "\t" << i << idx[i];
    }
#endif
}

void QskRoundedRectRenderer::renderRectellipse( const QRectF& rect,
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& border,
    const QskBoxBorderColors& borderColors, const QskGradient& gradient,
    QSGGeometry& geometry )
{
    const Metrics metrics( rect, shape, border );
    const auto isTilted = gradient.linearDirection().isTilted();

    int fillLineCount = 0;

    if ( !metrics.innerQuad.isEmpty() && gradient.isVisible() )
    {
        if ( metrics.isTotallyCropped )
        {
            // degenerated to a rectangle

            fillLineCount = gradient.stepCount() + 1;

#if 1
            if ( isTilted )
            {
                if ( metrics.centerQuad.width == metrics.centerQuad.height )
                {
                    if ( !gradient.hasStopAt( 0.5 ) )
                        fillLineCount++;
                }
                else
                {
                    // we might need extra lines for the corners
                    fillLineCount += 2;
                }
            }
#endif
        }
        else
        {
            fillLineCount = qskFillLineCount( metrics, gradient );
        }
    }

    const int stepCount = metrics.corner[ 0 ].stepCount;

    int borderLineCount = 0;

    if ( borderColors.isVisible() && metrics.innerQuad != metrics.outerQuad )
    {
        borderLineCount = 4 * ( stepCount + 1 ) + 1;

        const int additionalLines =
            additionalGradientStops( borderColors.left() )
            + additionalGradientStops( borderColors.top() )
            + additionalGradientStops( borderColors.right() )
            + additionalGradientStops( borderColors.bottom() );

        borderLineCount += additionalLines;
    }

    int lineCount = borderLineCount + fillLineCount;

    bool extraLine = false;
    if ( borderLineCount > 0 && fillLineCount > 0 )
    {
        if ( !gradient.isMonochrome() && isTilted )
        {
            /*
                The filling ends at 45° and we have no implementation
                for creating the border from there. So we need to
                insert an extra dummy line to connect fill and border
             */
            extraLine = true;
            lineCount++;
        }
    }

    auto lines = allocateLines< ColoredLine >( geometry, lineCount );

    bool fillRandom = true;
    if ( fillLineCount > 0 )
    {
        if ( metrics.isTotallyCropped )
        {
            fillRandom = false;
        }
        else if ( !gradient.isMonochrome() )
        {
            if ( gradient.stepCount() > 1 || isTilted )
                fillRandom = false;
        }

        if ( fillRandom )
        {
            if ( !metrics.isRadiusRegular )
            {
                /*
                    When we have a combination of corners with the same
                    or no radius we could use the faster random algo: TODO ...
                 */
                fillRandom = false;
            }
        }
    }

    if ( ( fillLineCount > 0 ) && ( borderLineCount > 0 ) )
    {
        if ( fillRandom )
        {
            qskRenderBoxRandom( metrics, borderColors,
                gradient, lines, lines + fillLineCount );
        }
        else
        {
            if ( metrics.isTotallyCropped )
            {
                QskRectRenderer::renderFill0( metrics.innerQuad,
                    gradient, fillLineCount, lines );
            }
            else if ( isTilted )
            {
                renderDiagonalFill( metrics, gradient, fillLineCount, lines );
            }
            else
            {
                qskRenderFillOrdered( metrics, gradient, fillLineCount, lines );
            }

            auto borderLines = lines + fillLineCount;
            if ( extraLine )
                borderLines++;

            const auto orientation = qskQtOrientation( gradient );
            qskRenderBorder( metrics, orientation, borderColors, borderLines );

            if ( extraLine )
            {
                const auto l = lines + fillLineCount;
                l[ 0 ].p1 = l[ -1 ].p2;
                l[ 0 ].p2 = l[ 1 ].p1;
            }
        }
    }
    else if ( fillLineCount > 0 )
    {
        if ( fillRandom )
        {
            qskRenderFillRandom( metrics, gradient, lines );
        }
        else
        {
            if ( metrics.isTotallyCropped )
            {
                QskRectRenderer::renderFill0( metrics.innerQuad,
                    gradient, fillLineCount, lines );
            }
            else if ( isTilted )
            {
                renderDiagonalFill( metrics, gradient, fillLineCount, lines );
            }
            else
            {
                qskRenderFillOrdered( metrics, gradient, fillLineCount, lines );
            }
        }
    }
    else if ( borderLineCount > 0 )
    {
#if 1
        /*
            In case of having an empty innerQuad and monochrome
            border colors, we could treat it like filling without border. TODO ...
         */
#endif
        qskRenderBorder( metrics, Qt::Vertical, borderColors, lines );
    }
}
