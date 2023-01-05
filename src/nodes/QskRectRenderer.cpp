/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxBorderColors.h"
#include "QskBoxBorderMetrics.h"
#include "QskRectRenderer.h"
#include "QskBoxRendererColorMap.h"
#include "QskGradient.h"
#include "QskGradientDirection.h"
#include "QskFunctions.h"
#include "QskVertex.h"

using namespace QskVertex;

namespace
{
    class HVRectIterator
    {
      public:
        inline HVRectIterator( const Quad& rect, const QLineF& vector )
            : m_rect( rect )
            , m_vertical( vector.x1() == vector.x2() )
        {
            if ( m_vertical )
            {
                m_t = rect.top + vector.y1() * rect.height;
                m_dt = vector.dy() * rect.height;

                m_values[0] = ( rect.top - m_t ) / m_dt;
                m_values[1] = ( rect.bottom - m_t ) / m_dt;
            }
            else
            {
                m_t = rect.left + vector.x1() * rect.width;
                m_dt = vector.dx() * rect.width;

                m_values[0] = ( rect.left - m_t ) / m_dt;
                m_values[1] = ( rect.right - m_t ) / m_dt;
            }
        }

        inline bool setGradientLine( qreal value, Color color, ColoredLine* line )
        {
            const auto v = m_t + value * m_dt;

            if ( m_vertical )
                line->setHLine( m_rect.left, m_rect.right, v, color );
            else
                line->setVLine( v, m_rect.top, m_rect.bottom, color );

            return true;
        }

        inline void setContourLine( Color color, ColoredLine* line )
        {
            if ( m_vertical )
            {
                const auto y = m_step ? m_rect.bottom : m_rect.top;
                line->setLine( m_rect.left, y, m_rect.right, y, color );
            }
            else
            {
                const auto x = m_step ? m_rect.right : m_rect.left;
                line->setLine( x, m_rect.top, x, m_rect.bottom, color );
            }
        }

        inline qreal value() const { return m_values[ m_step ? 1 : 0 ]; }
        inline bool advance() { return m_step++ == 0; }

      private:
        const Quad& m_rect;
        qreal m_t, m_dt;
        qreal m_values[2];
        const bool m_vertical;
        int m_step = 0;
    };

    class DRectIterator
    {
      public:
        inline DRectIterator( const Quad& quad, const QLineF& vector )
        {
            m_v.x = quad.left + vector.x1() * quad.width;
            m_v.y = quad.top + vector.y1() * quad.height;
            m_v.dx = vector.dx() * quad.width;
            m_v.dy = vector.dy() * quad.height;

            /*
                We calculate the values at the corners and order them
                in increasing order
             */

            const qreal lx = ( quad.left - m_v.x ) * m_v.dx;
            const qreal rx = ( quad.right - m_v.x ) * m_v.dx;
            const qreal ty = ( quad.top - m_v.y ) * m_v.dy;
            const qreal by = ( quad.bottom - m_v.y ) * m_v.dy;

            const qreal dot = m_v.dx * m_v.dx + m_v.dy * m_v.dy;

            const qreal tl = ( lx + ty ) / dot;
            const qreal tr = ( rx + ty ) / dot;
            const qreal bl = ( lx + by ) / dot;
            const qreal br = ( rx + by ) / dot;

            if ( ( m_v.dy >= 0.0 ) == ( m_v.dx >= 0.0 ) )
            {
                m_corners[0] = { { quad.left, quad.top }, tl };
                m_corners[1] = { { quad.right, quad.top }, tr };
                m_corners[2] = { { quad.left, quad.bottom }, bl };
                m_corners[3] = { { quad.right, quad.bottom }, br };
            }
            else
            {
                m_corners[0] = { { quad.left, quad.bottom }, bl };
                m_corners[1] = { { quad.right, quad.bottom }, br };
                m_corners[2] = { { quad.left, quad.top }, tl };
                m_corners[3] = { { quad.right, quad.top }, tr };
            }

            if ( m_corners[0].value > m_corners[3].value )
                qSwap( m_corners[0], m_corners[3] );

            if ( m_corners[1].value > m_corners[2].value )
                qSwap( m_corners[1], m_corners[2] );
        }

        inline bool setGradientLine( qreal value, Color color, ColoredLine* line )
        {
            const qreal m = m_v.dy / m_v.dx;

            const qreal x = m_v.x + m_v.dx * value;
            const qreal y = m_v.y + m_v.dy * value;

            const bool on = m_corners[0].pos.x() == m_corners[1].pos.x();

            QPointF p1, p2;

            switch( m_step )
            {
                case 1:
                {
                    p1 = p2 = m_corners[0].pos;

                    if ( on )
                    {
                        p1.ry() = y + ( x - p1.x() ) / m;
                        p2.rx() = x + ( y - p2.y() ) * m;
                    }
                    else
                    {
                        p1.rx() = x + ( y - p1.y() ) * m;
                        p2.ry() = y + ( x - p2.x() ) / m;
                    }

                    break;
                }
                case 2:
                {
                    p1 = m_corners[1].pos;
                    p2 = m_corners[0].pos;

                    if ( on )
                    {
                        p1.rx() = x + ( y - p1.y() ) * m;
                        p2.rx() = x + ( y - p2.y() ) * m;
                    }
                    else
                    {
                        p1.ry() = y + ( x - p1.x() ) / m;
                        p2.ry() = y + ( x - p2.x() ) / m;
                    }

                    break;
                }
                case 3:
                {
                    p1 = m_corners[1].pos;
                    p2 = m_corners[2].pos;

                    if ( on )
                    {
                        p1.rx() = x + ( y - p1.y() ) * m;
                        p2.ry() = y + ( x - p2.x() ) / m;
                    }
                    else
                    {
                        p1.ry() = y + ( x - p1.x() ) / m;
                        p2.rx() = x + ( y - p2.y() ) * m;
                    }
                    break;
                }
            }

            if ( p1.x() < p2.x() )
                line->setLine( p1.x(), p1.y(), p2.x(), p2.y(), color );
            else
                line->setLine( p2.x(), p2.y(), p1.x(), p1.y(), color );

            return true;
        }

        inline void setContourLine( Color color, ColoredLine* line )
        {
            if( m_step == 0 || m_step == 3 )
            {
                const auto& p = m_corners[ m_step ].pos;
                line->setLine( p.x(), p.y(), p.x(), p.y(), color );
            }
            else
            {
                const qreal m = m_v.dy / m_v.dx;

                auto p1 = m_corners[ m_step - 1 ].pos;
                const auto& p2 = m_corners[ m_step ].pos;

                if ( p1.x() == m_corners[ m_step + 1 ].pos.x() )
                    p1.ry() = p2.y() + ( p2.x() - p1.x() ) / m;
                else
                    p1.rx() = p2.x() + ( p2.y() - p1.y() ) * m;

                if ( p1.x() <= p2.x() )
                    line->setLine( p1.x(), p1.y(), p2.x(), p2.y(), color );
                else
                    line->setLine( p2.x(), p2.y(), p1.x(), p1.y(), color );
            }
        }

        inline qreal value() const
        {
            return m_corners[ m_step ].value;
        }

        inline bool advance()
        {
            return ++m_step <= 3;
        }

      private:
        struct { qreal x, y, dx, dy; } m_v;
        struct { QPointF pos; qreal value; } m_corners[4];

        int m_step = 0;
    };
}

static ColoredLine* qskAddFillLines( const Quad& rect,
    const QskGradient& gradient, int lineCount, ColoredLine* line )
{
    const auto dir = gradient.linearDirection();

    if ( dir.isTilted() )
    {
        DRectIterator it( rect, dir.vector() );
        line = QskVertex::fillOrdered( it, gradient, lineCount, line );
    }
    else
    {
        HVRectIterator it( rect, dir.vector() );
        line = QskVertex::fillOrdered( it, gradient, lineCount, line );
    }

    return line;
}

template< class Line >
static inline Line* qskAddBorderLines(
    const Quad& out, const Quad& in, Color color, Line* line )
{
    line[0].setLine( in.right, in.bottom, out.right, out.bottom, color );
    line[1].setLine( in.left, in.bottom, out.left, out.bottom, color );
    line[2].setLine( in.left, in.top, out.left, out.top, color );
    line[3].setLine( in.right, in.top, out.right, out.top, color );
    line[4] = line[ 0 ];

    return line + 5;
}

template< class Line >
static inline Line* qskAddBorderLines(
    const Quad& out, const Quad& in,
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

    return line;
}

void QskRectRenderer::renderBorder( const QRectF& rect,
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
    qskAddBorderLines( out, in, Color(), line );
}

void QskRectRenderer::renderFill0( const QskVertex::Quad& rect,
    const QskGradient& gradient, int lineCount, QskVertex::ColoredLine* line )
{
    qskAddFillLines( rect, gradient, lineCount, line );
}

void QskRectRenderer::renderFill( const QRectF& rect,
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

void QskRectRenderer::renderRect( const QRectF& rect,
    const QskBoxBorderMetrics& border, const QskBoxBorderColors& borderColors,
    const QskGradient& gradient, QSGGeometry& geometry )
{
    const Quad out = rect;
    const Quad in = qskValidOrEmptyInnerRect( rect, border.widths() );

    int fillLineCount = 0;
    if ( gradient.isVisible() && !in.isEmpty() )
    {
        fillLineCount = gradient.stepCount() + 1;

        if ( !gradient.isMonochrome() && gradient.linearDirection().isTilted() )
            fillLineCount += 2; // contour lines for the corners
    }

    int borderLineCount = 0;
    if ( in != out && borderColors.isVisible() )
    {
        // We can build a rectangular border from the 4 diagonal
        // lines at the corners, but need an additional line
        // for closing the border.

        borderLineCount = 4 + 1;

        if ( !borderColors.isMonochrome() )
        {
            // we might need extra lines to separate colors
            // at the non closing corners

            // ### As an optimization we could check orientation and colors
            // to test whether colors are the same
            const int additionalLines = -1
                + borderColors.left().stepCount()
                + borderColors.top().stepCount()
                + borderColors.right().stepCount()
                + borderColors.bottom().stepCount();

            borderLineCount += qMax( additionalLines, 0 );
        }
    }

    const auto lines = allocateLines< ColoredLine >(
        geometry, borderLineCount + fillLineCount );

    auto l = lines;

    if ( fillLineCount > 0 )
    {
        if ( gradient.isMonochrome() )
        {
            const auto c = gradient.rgbStart();

            ( l++ )->setHLine( rect.left(), rect.right(), rect.top(), c );
            ( l++ )->setHLine( rect.left(), rect.right(), rect.bottom(), c );
        }
        else
        {
            const auto dir = gradient.linearDirection();

            if ( gradient.stepCount() <= 1 && !dir.isTilted() )
            {
                const auto c1 = gradient.rgbStart();
                const auto c2 = gradient.rgbEnd();

                if ( dir.isVertical() )
                {
                    ( l++ )->setHLine( rect.left(), rect.right(), rect.top(), c1 );
                    ( l++ )->setHLine( rect.left(), rect.right(), rect.bottom(), c2 );
                }
                else
                {
                    ( l++ )->setVLine( rect.left(), rect.top(), rect.bottom(), c1 );
                    ( l++ )->setVLine( rect.right(), rect.top(), rect.bottom(), c2 );
                }
            }
            else
            {
                l = qskAddFillLines( in, gradient, fillLineCount, l );
            }
        }

        Q_ASSERT( l - lines == fillLineCount );
    }

    if ( borderLineCount > 0 )
    {
        if ( borderColors.isMonochrome() )
        {
            const auto rgb = borderColors.left().rgbStart();
            l = qskAddBorderLines( rect, in, Color( rgb ), l );
        }
        else
        {
            l = qskAddBorderLines( rect, in, borderColors, l );
        }

        Q_ASSERT( l - lines == borderLineCount + fillLineCount );
    }
}
