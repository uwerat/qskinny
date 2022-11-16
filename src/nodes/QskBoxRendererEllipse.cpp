/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxRenderer.h"

#include "QskBoxBorderColors.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxRendererColorMap.h"
#include "QskBoxShapeMetrics.h"
#include "QskGradientDirection.h"

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

    class ArcIterator
    {
      public:
        inline ArcIterator() = default;

        inline ArcIterator( int stepCount, bool inverted = false )
        {
            reset( stepCount, inverted );
        }

        void reset( int stepCount, bool inverted )
        {
            m_inverted = inverted;

            if ( inverted )
            {
                m_cos = 1.0;
                m_sin = 0.0;
            }
            else
            {
                m_cos = 0.0;
                m_sin = 1.0;
            }

            m_stepIndex = 0;
            m_stepCount = stepCount;

            const double angleStep = M_PI_2 / stepCount;
            m_cosStep = qFastCos( angleStep );
            m_sinStep = qFastSin( angleStep );
        }

        inline bool isInverted() const { return m_inverted; }

        inline double cos() const { return m_cos; }
        inline double sin() const { return m_inverted ? -m_sin : m_sin; }

        inline int step() const { return m_stepIndex; }
        inline int stepCount() const { return m_stepCount; }
        inline bool isDone() const { return m_stepIndex > m_stepCount; }

        inline void increment()
        {
            const double cos0 = m_cos;

            m_cos = m_cos * m_cosStep + m_sin * m_sinStep;
            m_sin = m_sin * m_cosStep - cos0 * m_sinStep;

            ++m_stepIndex;
        }

        inline void operator++() { increment(); }

        static int segmentHint( double radius )
        {
            const double arcLength = radius * M_PI_2;
            return qBound( 3, qCeil( arcLength / 3.0 ), 18 ); // every 3 pixels
        }

      private:
        double m_cos;
        double m_sin;

        int m_stepIndex;
        double m_cosStep;
        double m_sinStep;

        int m_stepCount;
        bool m_inverted;
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
        inline BorderValuesUniform( const QskBoxRenderer::Metrics& metrics )
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
        const QskBoxRenderer::Metrics::Corner& m_corner;
        qreal m_dx1, m_dy1, m_dx2, m_dy2;
    };

    class BorderValues
    {
      public:
        inline BorderValues( const QskBoxRenderer::Metrics& metrics )
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
        bool m_uniform;
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

    class FillValues
    {
      public:
        inline FillValues( const QskBoxRenderer::Metrics& metrics )
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
            }
        }

        inline void setAngle( qreal cos, qreal sin )
        {
            m_inner[ 0 ].setAngle( cos, sin );
            m_inner[ 1 ].setAngle( cos, sin );
            m_inner[ 2 ].setAngle( cos, sin );
            m_inner[ 3 ].setAngle( cos, sin );
        }

        inline qreal dx( int pos ) const { return m_inner[ pos ].dx; }
        inline qreal dy( int pos ) const { return m_inner[ pos ].dy; }

      private:
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
    };
}

namespace
{
    class VRectEllipseIterator
    {
      public:
        VRectEllipseIterator( const QskBoxRenderer::Metrics& metrics )
            : m_metrics( metrics )
            , m_values( metrics )
        {
            const auto& c = metrics.corner;

            m_v[ 0 ].left = c[ TopLeft ].centerX;
            m_v[ 0 ].right = c[ TopRight ].centerX;
            m_v[ 0 ].y = metrics.innerQuad.top;

            m_v[ 1 ] = m_v[ 0 ];

            m_leadingCorner = ( c[ TopLeft ].stepCount >= c[ TopRight ].stepCount )
                ? TopLeft : TopRight;

            m_arcIterator.reset( c[ m_leadingCorner ].stepCount, false );
        }

        template< class ColorIterator >
        inline void setGradientLine( const ColorIterator& it, ColoredLine* line )
        {
            const qreal y = it.value();
            const qreal f = ( y - m_v[ 0 ].y ) / ( m_v[ 1 ].y - m_v[ 0 ].y );
            const qreal left = m_v[ 0 ].left + f * ( m_v[ 1 ].left - m_v[ 0 ].left );
            const qreal right = m_v[ 0 ].right + f * ( m_v[ 1 ].right - m_v[ 0 ].right );

            line->setLine( left, y, right, y, it.color() );
        }

        template< class ColorIterator >
        inline void setContourLine( const ColorIterator& it, ColoredLine* line )
        {
            line->setLine( m_v[ 1 ].left, m_v[ 1 ].y,
                m_v[ 1 ].right, m_v[ 1 ].y, it.colorAt( m_v[ 1 ].y ) );
        }

        inline qreal value() const
        {
            return m_v[ 1 ].y;
        }

        inline bool advance()
        {
            const auto& centerQuad = m_metrics.centerQuad;
            const auto& c = m_metrics.corner;

            if ( m_arcIterator.step() == m_arcIterator.stepCount() )
            {
                if ( m_arcIterator.isInverted() )
                    return false;

                m_leadingCorner = ( c[ BottomLeft ].stepCount >= c[ BottomRight ].stepCount )
                    ? BottomLeft : BottomRight;

                m_arcIterator.reset( c[ m_leadingCorner ].stepCount, true );

                if ( centerQuad.top < centerQuad.bottom )
                {
                    m_v[ 0 ] = m_v[ 1 ];

                    m_v[ 1 ].left = m_metrics.innerQuad.left;
                    m_v[ 1 ].right = m_metrics.innerQuad.right;
                    m_v[ 1 ].y = centerQuad.bottom;

                    return true;
                }
            }

            m_arcIterator.increment();
            m_values.setAngle( m_arcIterator.cos(), m_arcIterator.sin() );

            m_v[ 0 ] = m_v[ 1 ];

            if ( m_arcIterator.isInverted() )
            {
                m_v[ 1 ].left = c[ BottomLeft ].centerX - m_values.dx( BottomLeft );
                m_v[ 1 ].right = c[ BottomRight ].centerX + m_values.dx( BottomRight );
                m_v[ 1 ].y = c[ m_leadingCorner ].centerY + m_values.dy( m_leadingCorner );
            }
            else
            {
                m_v[ 1 ].left = c[ TopLeft ].centerX - m_values.dx( TopLeft );
                m_v[ 1 ].right = c[ TopRight ].centerX + m_values.dx( TopRight );
                m_v[ 1 ].y = c[ m_leadingCorner ].centerY - m_values.dy( m_leadingCorner );
            }

            return true;
        }

      private:
        const QskBoxRenderer::Metrics& m_metrics;
        ArcIterator m_arcIterator;
        int m_leadingCorner;
        FillValues m_values;
        struct { qreal left, right, y; } m_v[ 2 ];
    };

    class HRectEllipseIterator
    {
      public:
        HRectEllipseIterator( const QskBoxRenderer::Metrics& metrics )
            : m_metrics( metrics )
            , m_values( metrics )
        {
            const auto& c = metrics.corner;

            m_v[ 0 ].top = c[ TopLeft ].centerY;
            m_v[ 0 ].bottom = c[ BottomLeft ].centerY;
            m_v[ 0 ].x = metrics.innerQuad.left;

            m_v[ 1 ] = m_v[ 0 ];

            m_leadingCorner = ( c[ TopLeft ].stepCount >= c[ BottomLeft ].stepCount )
                ? TopLeft : BottomLeft;

            m_arcIterator.reset( c[ m_leadingCorner ].stepCount, true );
        }

        template< class ColorIterator >
        inline void setGradientLine( const ColorIterator& it, ColoredLine* line )
        {
            const qreal x = it.value();
            const qreal f = ( x - m_v[ 0 ].x ) / ( m_v[ 1 ].x - m_v[ 0 ].x );
            const qreal top = m_v[ 0 ].top + f * ( m_v[ 1 ].top - m_v[ 0 ].top );
            const qreal bottom = m_v[ 0 ].bottom + f * ( m_v[ 1 ].bottom - m_v[ 0 ].bottom );

            line->setLine( x, top, x, bottom, it.color() );
        }

        template< class ColorIterator >
        inline void setContourLine( const ColorIterator& it, ColoredLine* line )
        {
            line->setLine( m_v[ 1 ].x, m_v[ 1 ].top,
                m_v[ 1 ].x, m_v[ 1 ].bottom, it.colorAt( m_v[ 1 ].x ) );
        }

        inline qreal value() const
        {
            return m_v[ 1 ].x;
        }

        inline bool advance()
        {
            const auto& centerQuad = m_metrics.centerQuad;
            const auto& c = m_metrics.corner;

            if ( m_arcIterator.step() == m_arcIterator.stepCount() )
            {
                if ( !m_arcIterator.isInverted() )
                    return false;

                m_leadingCorner = ( c[ TopRight ].stepCount >= c[ BottomRight ].stepCount )
                    ? TopRight : BottomRight;

                m_arcIterator.reset( c[ m_leadingCorner ].stepCount, false );
                if ( centerQuad.left < centerQuad.right )
                {
                    m_v[ 0 ] = m_v[ 1 ];

                    m_v[ 1 ].top = m_metrics.innerQuad.top;
                    m_v[ 1 ].bottom = m_metrics.innerQuad.bottom;
                    m_v[ 1 ].x = centerQuad.right;

                    return true;
                }
            }

            m_arcIterator.increment();
            m_values.setAngle( m_arcIterator.cos(), m_arcIterator.sin() );

            m_v[ 0 ] = m_v[ 1 ];

            if ( m_arcIterator.isInverted() )
            {
                m_v[ 1 ].top = c[ TopLeft ].centerY - m_values.dy( TopLeft );
                m_v[ 1 ].bottom = c[ BottomLeft ].centerY + m_values.dy( BottomLeft );
                m_v[ 1 ].x = c[ m_leadingCorner ].centerX - m_values.dx( m_leadingCorner );
            }
            else
            {
                m_v[ 1 ].top = c[ TopRight ].centerY - m_values.dy( TopRight );
                m_v[ 1 ].bottom = c[ BottomRight ].centerY + m_values.dy( BottomRight );
                m_v[ 1 ].x = c[ m_leadingCorner ].centerX + m_values.dx( m_leadingCorner );
            }

            return true;
        }

      private:
        const QskBoxRenderer::Metrics& m_metrics;
        ArcIterator m_arcIterator;
        int m_leadingCorner;
        FillValues m_values;
        struct { qreal top, bottom, x; } m_v[ 2 ];
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
        inline BorderMapGradient( int stepCount, const QskGradient& gradient1, const QskGradient& gradient2 )
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
        inline Stroker( const QskBoxRenderer::Metrics& metrics )
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
                const auto &stop = stops.first();

                if ( stop.position() > 0.0 )
                    setAdditionalLine( x11, y11, x12, y12, dx1, dy1, dx2, dy2, stop, line-- );
            }

            for( int i = 1; i < stops.count() - 1; i++ )
            {
                setAdditionalLine( x11, y11, x12, y12, dx1, dy1, dx2, dy2, stops[i], line-- );
            }

            {
                const auto &stop = stops.last();

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
        const QskBoxRenderer::Metrics& m_metrics;
    };
}

static inline Qt::Orientation qskQtOrientation( const QskGradient& gradient )
{
    return gradient.linearDirection().isVertical() ? Qt::Vertical : Qt::Horizontal;
}

static inline int qskFillLineCount(
    const QskBoxRenderer::Metrics& metrics, const QskGradient& gradient )
{
    const int stepCount = metrics.corner[ 0 ].stepCount;

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
        lineCount += 2 * ( stepCount + 1 );

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
    const QskBoxRenderer::Metrics& metrics,
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
        Stroker< Line, BorderValues > stroker( metrics );
        stroker.createLines( orientation, borderLines,
            borderMapTL, borderMapTR, borderMapBL, borderMapBR,
            fillLines, fillMap );
    }
}

template< class Line, class BorderMap, class FillMap >
static inline void qskRenderLines(
    const QskBoxRenderer::Metrics& metrics, Qt::Orientation orientation,
    Line* borderLines, const BorderMap& borderMap, Line* fillLines,
    const FillMap& fillMap )
{
    qskRenderLines( metrics, orientation, borderLines,
        borderMap, borderMap, borderMap, borderMap, fillLines, fillMap );
}

template< class Line, class BorderMap >
static inline void qskRenderBorderLines(
    const QskBoxRenderer::Metrics& metrics,
    Qt::Orientation orientation, Line* lines,
    const BorderMap& borderMapTL, const BorderMap& borderMapTR,
    const BorderMap& borderMapBL, const BorderMap& borderMapBR )
{
    qskRenderLines( metrics, orientation, lines, borderMapTL, borderMapTR,
        borderMapBL, borderMapBR, static_cast< Line* >( nullptr ), ColorMapNone() );
}

template< class Line, class BorderMap >
static inline void qskRenderBorderLines(
    const QskBoxRenderer::Metrics& metrics,
    Qt::Orientation orientation, Line* lines, const BorderMap& borderMap )
{
    qskRenderBorderLines( metrics, orientation, lines,
        borderMap, borderMap, borderMap, borderMap );
}

template< class Line, class FillMap >
static inline void qskRenderFillLines( const QskBoxRenderer::Metrics& metrics,
    Qt::Orientation orientation, Line* lines, const FillMap& fillMap )
{
    qskRenderLines( metrics, orientation,
        static_cast< Line* >( nullptr ), BorderMapNone(), lines, fillMap );
}

static inline void qskRenderBorder( const QskBoxRenderer::Metrics& metrics,
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
    const QskBoxRenderer::Metrics& metrics,
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
    const QskBoxRenderer::Metrics& metrics, const QskBoxBorderColors& borderColors,
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
    const QskBoxRenderer::Metrics& metrics,
    const QskGradient& gradient, ColoredLine* lines )
{
    const auto& r = metrics.innerQuad;

    /*
        The algo for irregular radii at opposite corners is not yet
        implemented TODO ...
     */

    if ( gradient.linearDirection().isHorizontal() )
    {
        HRectEllipseIterator it( metrics );
        QskVertex::fillOrdered( it, r.left, r.right, gradient, lines );
    }
    else
    {
        VRectEllipseIterator it( metrics );
        QskVertex::fillOrdered( it, r.top, r.bottom, gradient, lines );
    }
}

QskBoxRenderer::Metrics::Metrics( const QRectF& rect,
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

void QskBoxRenderer::renderRectellipseBorder(
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

void QskBoxRenderer::renderRectellipseFill(
    const QRectF& rect, const QskBoxShapeMetrics& shape,
    const QskBoxBorderMetrics& border, QSGGeometry& geometry )
{
    const Metrics metrics( rect, shape, border );

    if ( ( metrics.innerQuad.width <= 0 ) || ( metrics.innerQuad.height <= 0 ) )
    {
        allocateLines< Line >( geometry, 0 );
        return;
    }

    if ( metrics.isTotallyCropped )
    {
        // degenerated to a rectangle

        const QRectF r( metrics.innerQuad.left, metrics.innerQuad.top,
            metrics.innerQuad.width, metrics.innerQuad.height );

        renderRectFill( r, QskBoxShapeMetrics(),
            QskBoxBorderMetrics(), geometry );

        return;
    }

    const int stepCount = metrics.corner[ 0 ].stepCount;

    int lineCount = 2 * stepCount;

    if ( metrics.centerQuad.top >= metrics.centerQuad.bottom )
    {
        // we need an extra line connecting the top/bottom corners
        lineCount++;
    }
    else
    {
        // for some reason we have 2 more lines for true ellipses ??
        lineCount += 2;
    }

    const auto line = allocateLines< Line >( geometry, lineCount );
    qskRenderFillLines( metrics, Qt::Vertical, line, ColorMapNone() );
}

void QskBoxRenderer::renderRectellipse( const QRectF& rect,
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& border,
    const QskBoxBorderColors& borderColors, const QskGradient& gradient,
    QSGGeometry& geometry )
{
    const Metrics metrics( rect, shape, border );

    int fillLineCount = 0;

    if ( !metrics.innerQuad.isEmpty() && gradient.isVisible() )
    {
        if ( metrics.isTotallyCropped )
        {
            // degenerated to a rectangle

            fillLineCount = gradient.stepCount() + 1;

#if 1
            // code copied from QskBoxRendererRect.cpp TODO ...

            if ( gradient.linearDirection().isTilted() )
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
        if ( !gradient.isMonochrome() && gradient.linearDirection().isTilted() )
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

    auto line = allocateLines< ColoredLine >( geometry, lineCount );

    bool fillRandom = true;
    if ( fillLineCount > 0 )
    {
        if ( metrics.isTotallyCropped )
        {
            fillRandom = false;
        }
        else if ( !gradient.isMonochrome() )
        {
            if ( gradient.stepCount() > 1 || gradient.linearDirection().isTilted() )
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
                gradient, line, line + fillLineCount );
        }
        else
        {
            if ( metrics.isTotallyCropped )
            {
                renderRectFill( metrics.innerQuad, gradient, line );
            }
            else if ( gradient.linearDirection().isTilted() )
            {
                renderDiagonalFill( metrics, gradient, fillLineCount, line );
            }
            else
            {
                qskRenderFillOrdered( metrics, gradient, line );
            }

            auto borderLines = line + fillLineCount;
            if ( extraLine )
                borderLines++;

            const auto orientation = qskQtOrientation( gradient );
            qskRenderBorder( metrics, orientation, borderColors, borderLines );

            if ( extraLine )
            {
                const auto l = line + fillLineCount;
                l[ 0 ].p1 = l[ -1 ].p2;
                l[ 0 ].p2 = l[ 1 ].p1;
            }
        }
    }
    else if ( fillLineCount > 0 )
    {
        if ( fillRandom )
        {
            qskRenderFillRandom( metrics, gradient, line );
        }
        else
        {
            if ( metrics.isTotallyCropped )
            {
                renderRectFill( metrics.innerQuad, gradient, line );
            }
            else if ( gradient.linearDirection().isTilted() )
            {
                renderDiagonalFill( metrics, gradient, fillLineCount, line );
            }
            else
            {
                qskRenderFillOrdered( metrics, gradient, line );
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
        qskRenderBorder( metrics, Qt::Vertical, borderColors, line );
    }
}
