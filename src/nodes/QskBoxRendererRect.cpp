/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxBorderColors.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxRenderer.h"
#include "QskBoxRendererColorMap.h"
#include "QskFunctions.h"
#include "QskGradientDirection.h"
#include "QskVertex.h"

using namespace QskVertex;

namespace
{
    class VRectIterator
    {
      public:
        inline VRectIterator( const QskBoxRenderer::Quad& rect )
            : m_rect( rect )
            , m_value( rect.top )
        {
        }

        template< class ColorIterator >
        inline void setGradientLine( const ColorIterator& it, ColoredLine* line )
        {
            line->setHLine( m_rect.left, m_rect.right, it.value(), it.color() );
        }

        template< class ColorIterator >
        inline void setContourLine( const ColorIterator& it, ColoredLine* line )
        {
            line->setHLine( m_rect.left, m_rect.right, m_value, it.colorAt( m_value ) );
        }

        inline qreal value() const
        {
            return m_value;
        }

        inline bool advance()
        {
            if ( m_value == m_rect.top )
            {
                m_value = m_rect.bottom;
                return true;
            }

            return false;
        }

      private:
        const QskBoxRenderer::Quad& m_rect;
        qreal m_value;
    };

    class HRectIterator
    {
      public:
        inline HRectIterator( const QskBoxRenderer::Quad& rect )
            : m_rect( rect )
            , m_value( rect.left )
        {
        }

        template< class ColorIterator >
        inline void setGradientLine( const ColorIterator& it, ColoredLine* line )
        {
            line->setVLine( it.value(), m_rect.top, m_rect.bottom, it.color() );
        }

        template< class ColorIterator >
        inline void setContourLine( const ColorIterator& it, ColoredLine* line )
        {
            line->setVLine( m_value, m_rect.top, m_rect.bottom, it.colorAt( m_value ) );
        }

        inline qreal value() const
        {
            return m_value;
        }

        inline bool advance()
        {
            if ( m_value == m_rect.left )
            {
                m_value = m_rect.right;
                return true;
            }

            return false;
        }

      private:
        const QskBoxRenderer::Quad& m_rect;
        qreal m_value;
    };

    class DSquareIterator
    {
      public:
        inline DSquareIterator( const QskBoxRenderer::Quad& rect )
            : m_rect( rect )
            , m_step( 0 )
        {
            Q_ASSERT( rect.width == rect.height );
        }

        template< class ColorIterator >
        inline void setGradientLine( const ColorIterator& it, ColoredLine* line )
        {
            const auto v = it.value();

            if ( v == 0.5 )
            {
                // this line is also a contour line, so we can skip it
                return;
            }
            else if ( v < 0.5 )
            {
                const qreal dt = m_rect.width * 2 * v;

                line->setLine( m_rect.left, m_rect.top + dt,
                    m_rect.left + dt, m_rect.top, it.color() );
            }
            else
            {
                const qreal dt = m_rect.width * 2 * ( v - 0.5 );
                line->setLine( m_rect.left + dt, m_rect.bottom,
                    m_rect.right, m_rect.top + dt, it.color() );
            }
        }

        template< class ColorIterator >
        inline void setContourLine( const ColorIterator& it, ColoredLine* line )
        {
            const auto color = it.colorAt( value() );
            const auto& r = m_rect;

            switch ( m_step )
            {
                case 0:
                {
                    line->setLine( r.left, r.top, r.left, r.top, color );
                    break;
                }
                case 1:
                {
                    line->setLine( r.left, r.bottom, r.right, r.top, color );
                    break;
                }
                case 2:
                {
                    line->setLine( r.right, r.bottom, r.right, r.bottom, color );
                    break;
                }
            }
        }

        inline qreal value() const
        {
            return m_step * 0.5;
        }

        inline bool advance()
        {
            return ++m_step <= 2;
        }

      private:
        const QskBoxRenderer::Quad& m_rect;
        int m_step;
    };

    class DRectIterator
    {
      public:
        inline DRectIterator( const QskBoxRenderer::Quad& rect )
            : m_rect( rect )
            , m_step( 0 )
        {
            const qreal w = rect.width;
            const qreal h = rect.height;

            Q_ASSERT( w != h );

            const qreal w2 = w * w;
            const qreal h2 = h * h;

            m_fx = ( w2 + h2 ) / w;
            m_fy = ( w2 + h2 ) / h;

            m_lx = m_rect.top - h2 / w;
            m_ly = m_rect.left - w2 / h;

            m_valueTR = w2 / ( w2 + h2 );
            m_valueBL = h2 / ( w2 + h2 );
        }

        template< class ColorIterator >
        inline void setGradientLine( const ColorIterator& it, ColoredLine* line )
        {
            const auto v = it.value();
            const auto color = it.color();
            const auto& r = m_rect;

            switch ( m_step )
            {
                case 1:
                {
                    const qreal dx = v * m_fx;
                    const qreal dy = v * m_fy;

                    line->setLine( r.left, r.top + dy, r.left + dx, r.top, color );
                    break;
                }
                case 2:
                {
                    if ( r.width > r.height )
                    {
                        const qreal dx = v * m_fx;
                        line->setLine( m_lx + dx, r.bottom, r.left + dx, r.top, color );
                    }
                    else
                    {
                        const qreal dy = v * m_fy;
                        line->setLine( r.left, r.top + dy, r.right, m_ly + dy, color );
                    }
                    break;
                }
                case 3:
                {
                    const qreal dx = v * m_fx;
                    const qreal dy = v * m_fy;

                    line->setLine( m_lx + dx, r.bottom, r.right, m_ly + dy, color );
                    break;
                }
            }
        }

        template< class ColorIterator >
        inline void setContourLine( const ColorIterator& it, ColoredLine* line )
        {
            const auto& r = m_rect;

            switch ( m_step )
            {
                case 0:
                {
                    line->setLine( r.left, r.top,
                        r.left, r.top, it.colorAt( 0.0 ) );

                    break;
                }
                case 1:
                {
                    if ( r.width >= r.height )
                    {
                        const qreal dx = m_valueBL * m_fx;

                        line->setLine( r.left, r.bottom,
                            r.left + dx, r.top, it.colorAt( m_valueBL ) );
                    }
                    else
                    {
                        const qreal dy = m_valueTR * m_fy;

                        line->setLine( r.left, r.top + dy,
                            r.right, r.top, it.colorAt( m_valueTR ) );
                    }

                    break;
                }
                case 2:
                {
                    if ( r.width >= r.height )
                    {
                        const qreal dx = m_valueTR * m_fx;

                        line->setLine( r.left + dx, r.bottom,
                            r.right, r.top, it.colorAt( m_valueTR ) );
                    }
                    else
                    {
                        const qreal dy = m_valueBL * m_fy;

                        line->setLine( r.left, r.bottom,
                            r.right, r.top + dy, it.colorAt( m_valueBL ) );
                    }

                    break;
                }
                case 3:
                {
                    line->setLine( r.right, r.bottom,
                        r.right, r.bottom, it.colorAt( 1.0 ) );

                    break;
                }
                default:
                {
                    Q_ASSERT( false );
                }
            }
        }

        inline qreal value() const
        {
            switch ( m_step )
            {
                case 0:
                    return 0.0;

                case 1:
                    return std::min( m_valueBL, m_valueTR );

                case 2:
                    return std::max( m_valueBL, m_valueTR );

                default:
                    return 1.0;
            }
        }

        inline bool advance()
        {
            return ++m_step <= 3;
        }

      private:
        const QskBoxRenderer::Quad& m_rect;

        qreal m_fx, m_fy;
        qreal m_lx, m_ly;
        qreal m_valueTR, m_valueBL;

        int m_step;
    };
}

static inline void qskCreateFillOrdered( const QskBoxRenderer::Quad& rect,
    const QskGradient& gradient, ColoredLine* line )
{
    const auto dir = gradient.linearDirection();

    if ( dir.isHorizontal() )
    {
        HRectIterator it( rect );
        line = QskVertex::fillOrdered( it, rect.left, rect.right, gradient, line );
    }
    else if ( dir.isVertical() )
    {
        VRectIterator it( rect );
        line = QskVertex::fillOrdered( it, rect.top, rect.bottom, gradient, line );
    }
    else
    {
        if ( rect.width == rect.height )
        {
            DSquareIterator it( rect );
            line = QskVertex::fillOrdered( it, 0.0, 1.0, gradient, line );
        }
        else
        {
            DRectIterator it( rect );
            line = QskVertex::fillOrdered( it, 0.0, 1.0, gradient, line );
        }
    }
}

template< class ColorMap, class Line >
static inline void qskCreateFillRandom( Qt::Orientation orientation,
    const QskBoxRenderer::Quad& r, const ColorMap& map, Line* line )
{
    if ( orientation == Qt::Vertical )
    {
        ( line++ )->setLine( r.left, r.top, r.right, r.top, map.colorAt( 0.0 ) );
        ( line++ )->setLine( r.left, r.bottom, r.right, r.bottom, map.colorAt( 1.0 ) );
    }
    else
    {
        ( line++ )->setLine( r.left, r.top, r.left, r.bottom, map.colorAt( 0.0 ) );
        ( line++ )->setLine( r.right, r.top, r.right, r.bottom, map.colorAt( 1.0 ) );
    }
}

template< class Line >
static inline void qskCreateBorderMonochrome(
    const QskBoxRenderer::Quad& out, const QskBoxRenderer::Quad& in, QRgb rgb, Line* line )
{
    qskCreateBorderMonochrome( out, in, Color( rgb ), line );
}

template< class Line >
static inline void qskCreateBorderMonochrome(
    const QskBoxRenderer::Quad& out, const QskBoxRenderer::Quad& in, Color color, Line* line )
{
    auto l = line;

    ( l++ )->setLine( in.right, in.bottom, out.right, out.bottom, color );
    ( l++ )->setLine( in.left, in.bottom, out.left, out.bottom, color );
    ( l++ )->setLine( in.left, in.top, out.left, out.top, color );
    ( l++ )->setLine( in.right, in.top, out.right, out.top, color );

    *l = line[ 0 ];
}

template< class Line >
static inline void qskCreateBorder(
    const QskBoxRenderer::Quad& out, const QskBoxRenderer::Quad& in,
    const QskBoxBorderColors& colors, Line* line )
{
    const qreal dx1 = in.right - in.left;
    const qreal dx2 = out.right - out.left;
    const qreal dy1 = in.top - in.bottom;
    const qreal dy2 = out.top - out.bottom;

    {
        const auto stops = colors.bottom().stops();

        if ( stops.first().position() > 0.0 )
        {
            ( line++ )->setLine( in.right, in.bottom,
                out.right, out.bottom, stops.first().rgb() );
        }

        for( const auto& stop : stops )
        {
            const qreal x1 = in.right - stop.position() * dx1;
            const qreal x2 = out.right - stop.position() * dx2;

            ( line++ )->setLine( x1, in.bottom, x2, out.bottom, stop.rgb() );
        }

        if ( stops.last().position() < 1.0 )
        {
            ( line++ )->setLine( in.left, in.bottom,
                out.left, out.bottom, stops.last().rgb() );
        }
    }

    {
        const auto stops = colors.left().stops();

        if ( stops.first().position() > 0.0 )
        {
            ( line++ )->setLine( in.left, in.bottom,
                out.left, out.bottom, stops.first().rgb() );
        }

        for( const auto& stop : stops )
        {
            const qreal y1 = in.bottom + stop.position() * dy1;
            const qreal y2 = out.bottom + stop.position() * dy2;

            ( line++ )->setLine( in.left, y1, out.left, y2, stop.rgb() );
        }

        if ( stops.last().position() < 1.0 )
        {
            ( line++ )->setLine( in.left, in.top,
                out.left, out.top, stops.last().rgb() );
        }
    }

    {
        const auto stops = colors.top().stops();

        if ( stops.first().position() > 0.0 )
        {
            ( line++ )->setLine( in.left, in.top,
                out.left, out.top, stops.first().rgb() );
        }

        for( const auto& stop : stops )
        {
            const qreal x1 = in.left + stop.position() * dx1;
            const qreal x2 = out.left + stop.position() * dx2;

            ( line++ )->setLine( x1, in.top, x2, out.top, stop.rgb() );
        }

        if ( stops.last().position() < 1.0 )
        {
            ( line++ )->setLine( in.right, in.top,
                out.right, out.top, stops.last().rgb() );
        }
    }

    {
        const auto stops = colors.right().stops();

        if ( stops.first().position() > 0.0 )
        {
            ( line++ )->setLine( in.right, in.top,
                out.right, out.top, stops.first().rgb() );
        }

        for( const auto& stop : stops )
        {
            const qreal y1 = in.bottom + ( 1 - stop.position() ) * dy1;
            const qreal y2 = out.bottom + ( 1 - stop.position() ) * dy2;

            ( line++ )->setLine( in.right, y1, out.right, y2, stop.rgb() );
        }

        if ( stops.last().position() < 1.0 )
        {
            ( line++ )->setLine( in.right, in.bottom,
                out.right, out.bottom, stops.last().rgb() );
        } 
    }
}

void QskBoxRenderer::renderRectBorder( const QRectF& rect,
    const QskBoxBorderMetrics& border, QSGGeometry& geometry )
{
    const Quad out = rect;
    const Quad in = qskValidOrEmptyInnerRect( rect, border.widths() );

    if ( out == in )
    {
        allocateLines< Line >( geometry, 0 );
        return;
    }

    const auto line = allocateLines< Line >( geometry, 4 + 1 );
    qskCreateBorderMonochrome( out, in, Color(), line );
}

void QskBoxRenderer::renderRectFill( const QRectF& rect, 
    const QskBoxBorderMetrics& border, QSGGeometry& geometry )
{
    const Quad quad = qskValidOrEmptyInnerRect( rect, border.widths() );

    if ( quad.isEmpty() )
    {
        geometry.allocate( 0 );
        return;
    }

    geometry.allocate( 4 );

    auto p = geometry.vertexDataAsPoint2D();
    p[0].set( quad.left, quad.top );
    p[1].set( quad.right, quad.top );
    p[2].set( quad.left, quad.bottom );
    p[3].set( quad.right, quad.bottom );
}

void QskBoxRenderer::renderRect( const QRectF& rect,
    const QskBoxBorderMetrics& border, const QskBoxBorderColors& borderColors,
    const QskGradient& gradient, QSGGeometry& geometry )
{
    const Quad out = rect;
    const Quad in = qskValidOrEmptyInnerRect( rect, border.widths() );

    int fillLineCount = 0;
    if ( gradient.isVisible() && !in.isEmpty() )
    {
        fillLineCount = gradient.stepCount() + 1;

        if ( gradient.linearDirection().isTilted() )
        {
            if ( in.width == in.height )
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
    }

    int borderLineCount = 0;
    if ( in != out )
    {
        const auto& bc = borderColors;

        if ( bc.isVisible() )
        {
            // We can build a rectangular border from the 4 diagonal
            // lines at the corners, but need an additional line
            // for closing the border.

            borderLineCount = 4 + 1;

            if ( !bc.isMonochrome() )
            {
                // we might need extra lines to separate colors
                // at the non closing corners

                // ### As an optimization we could check orientation and colors
                // to test whether colors are the same
                const int additionalLines = -1
                    + bc.left().stepCount()
                    + bc.top().stepCount()
                    + bc.right().stepCount()
                    + bc.bottom().stepCount();

                borderLineCount += qMax( additionalLines, 0 );
            }
        }
    }

    auto line = allocateLines< ColoredLine >( geometry, borderLineCount + fillLineCount );

    if ( fillLineCount > 0 )
    {
        if ( gradient.isMonochrome() )
        {
            const ColorMapSolid colorMap( gradient );
            qskCreateFillRandom( Qt::Vertical, in, colorMap, line );
        }
        else
        {
            bool fillRandom = gradient.stepCount() <= 1;
            if ( fillRandom )
            {
                /*
                    Not necessarily a requirement for being ordered,
                    but we didn't implement a random fill algo for
                    diagonal gradients yet.
                 */
                fillRandom = !gradient.linearDirection().isTilted();
            }

            if ( fillRandom )
            {
                const auto orientation = gradient.linearDirection().isVertical()
                    ? Qt::Vertical : Qt::Horizontal;

                const ColorMapGradient colorMap( gradient );
                qskCreateFillRandom( orientation, in, colorMap, line );
            }
            else
            {
                qskCreateFillOrdered( in, gradient, line );
            }
        }
    }

    if ( borderLineCount > 0 )
    {
        const auto& bc = borderColors;
        auto fillLines = line + fillLineCount;

        if ( bc.isMonochrome() )
        {
            const auto rgb = bc.left().rgbStart();
            qskCreateBorderMonochrome( rect, in, rgb, fillLines );
        }
        else
        {
            qskCreateBorder( rect, in, bc, fillLines );
        }
    }
}

void QskBoxRenderer::renderRectFill( const QskBoxRenderer::Quad& rect,
    const QskGradient& gradient, QskVertex::ColoredLine* line )
{
    qskCreateFillOrdered( rect, gradient, line );
}

bool QskBoxRenderer::isGradientSupported( const QskGradient& gradient )
{
    if ( !gradient.isVisible() || gradient.isMonochrome() )
        return true;

    switch( gradient.type() )
    {
        case QskGradient::Linear:
        {
            auto dir = gradient.linearDirection();

            if ( dir.isTilted() )
            {
                // only diagonal from topLeft to bottomRight
                return ( dir.x1() == dir.x2() ) && ( dir.y1() == dir.y2() );
            }

            return true;
        }
        case QskGradient::Radial:
        case QskGradient::Conic:
        {
            return false;
        }
        default:
        {
            return true;
        }
    }
}

