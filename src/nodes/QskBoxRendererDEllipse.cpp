/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxRenderer.h"
#include "QskBoxRendererColorMap.h"
#include "QskGradient.h"
#include "QskVertex.h"

#include <qmath.h>

namespace
{
    using namespace QskVertex;

    class ContourLine
    {
      public:
        inline void setLine( qreal x1, qreal y1, qreal value1,
            qreal x2, qreal y2, qreal value2 )
        {
            p1.x = x1;
            p1.y = y1;
            p1.v = value1;

            p2.x = x2;
            p2.y = y2;
            p2.v = value2;
        }

        inline QPointF pointAt( qreal value ) const
        {
            const qreal r = ( value - p1.v ) / ( p2.v - p1.v );
            return QPointF( p1.x + r * ( p2.x - p1.x ), p1.y + r * ( p2.y - p1.y ) );
        }

        struct
        {
            qreal x, y, v;
        } p1, p2;
    };

    class ValueCurve
    {
      public:
        ValueCurve( const QskBoxRenderer::Metrics& m )
        {
            /*
                The slopes of the value line and those for the fill lines.
             */
            const qreal mv = -( m.innerQuad.width / m.innerQuad.height );
            const qreal md = m.innerQuad.height / m.innerQuad.width;

            /*
                first we find the point, where the tangent line
                with the slope of md touches the top left border
                of the ellipse around the top left corner point
             */

            qreal xt, yt;
            {
                const auto& c = m.corner[ Qt::TopLeftCorner ];

                const qreal k = c.radiusInnerY / c.radiusInnerX * md;
                const qreal u = ::sqrt( 1.0 + k * k );

                const qreal dx = c.radiusInnerX / u;
                const qreal dy = ::sqrt( 1.0 - 1.0 / ( u * u ) ) * c.radiusInnerY;

                xt = c.centerX - dx;
                yt = c.centerY - dy;
            }

            /*
                the cutting point between the tangent and the diagonal of the
                fill rectangle is the origin of our line, where we iterate
                the values along.
             */

            qreal left, top, right;
            {
                const auto& r = m.innerQuad;

                left = ( yt - r.top - mv * xt + md * r.left ) / ( md - mv );

                const qreal dx = left - r.left;

                top = r.top + md * dx;
                right = r.right - dx;
            }

            /*
                Finally we can precaclculate the coefficients needed for
                evaluating points in valueAt.
             */

            {
                const qreal k = mv + 1.0 / mv;
                const qreal r = top + left / mv;
                const qreal w = right - left;

                m_coeff_0 = ( r / k - left ) / w;
                m_coeff_y = -1.0 / ( k * w );
                m_coeff_x = -mv * m_coeff_y;
            }
        }

        inline qreal valueAt( qreal x, qreal y ) const
        {
            // The value, where the perpendicular runs through x,y

            return m_coeff_0 + x * m_coeff_x + y * m_coeff_y;
        }

      private:
        qreal m_coeff_0, m_coeff_x, m_coeff_y;
    };

    class ContourIterator
    {
      public:
        ContourIterator()
            : m_clockwise( true )
            , m_isLeading( true )
            , m_isDone( false )
        {
        }

        void setup( const QskBoxRenderer::Metrics& metrics,
            bool isLeading, bool clockwise,
            qreal cos, qreal cosStep, qreal sin, qreal sinStep,
            qreal x1, qreal y1, qreal v1, qreal x2, qreal y2, qreal v2 )
        {
            m_cos = cos;
            m_sin = sin;

            m_cosStep = cosStep;
            m_sinStep = sinStep;

            m_stepInv1 = m_sinStep / m_cosStep;
            m_stepInv2 = m_cosStep + m_sinStep * m_stepInv1;

            m_contourLine.setLine( x1, y1, v1, x2, y2, v2 );

            m_clockwise = clockwise;
            m_isLeading = isLeading;
            m_isDone = false;

            m_corner = Qt::TopLeftCorner;

            if ( clockwise )
            {
                if ( x2 >= metrics.corner[ Qt::TopRightCorner ].centerX )
                    setCorner( Qt::TopRightCorner, metrics );
            }
            else
            {
                if ( y2 >= metrics.corner[ Qt::BottomLeftCorner ].centerY )
                    setCorner( Qt::BottomLeftCorner, metrics );
            }
        }

        inline bool isClockwise() const { return m_clockwise; }
        inline bool isDone() const { return m_isDone; }
        inline qreal value() const { return m_contourLine.p2.v; }

        inline const ContourLine& contourLine() const { return m_contourLine; }

        inline void advance( const QskBoxRenderer::Metrics& metrics, const ValueCurve& curve )
        {
            if ( m_isDone )
                return;

            using namespace Qt;

            const auto& corners = metrics.corner;
            const auto& c = corners[ m_corner ];

            m_contourLine.p1 = m_contourLine.p2;

            auto& p = m_contourLine.p2;

            if ( m_clockwise )
            {
                switch ( m_corner )
                {
                    case TopLeftCorner:
                    {
                        if ( p.x >= c.centerX )
                        {
                            p.x = corners[ TopRightCorner ].centerX;
                            p.y = metrics.innerQuad.top;

                            setCorner( TopRightCorner, metrics );
                        }
                        else
                        {
                            decrement();
                            p.x = c.centerX - m_cos * c.radiusInnerX;
                            p.y = c.centerY - m_sin * c.radiusInnerY;

                            if ( p.x >= corners[ TopRightCorner ].centerX )
                                setCorner( TopRightCorner, metrics );
                        }

                        break;
                    }
                    case TopRightCorner:
                    {
                        if ( p.y >= c.centerY )
                        {
                            p.x = metrics.innerQuad.right;
                            p.y = corners[ BottomRightCorner ].centerY;

                            setCorner( BottomRightCorner, metrics );
                        }
                        else
                        {
                            increment();
                            p.x = c.centerX + m_cos * c.radiusInnerX;
                            p.y = c.centerY - m_sin * c.radiusInnerY;

                            if ( p.y >= corners[ BottomRightCorner ].centerY )
                                setCorner( BottomRightCorner, metrics );
                        }

                        break;
                    }
                    case BottomRightCorner:
                    {
                        // we are bottom/right
                        increment();

                        p.x = c.centerX + m_cos * c.radiusInnerX;
                        p.y = c.centerY + m_sin * c.radiusInnerY;

                        break;
                    }
                    default:
                    {
                        Q_ASSERT( false );
                    }
                }
            }
            else
            {
                switch ( m_corner )
                {
                    case TopLeftCorner:
                    {
                        if ( p.y >= c.centerY )
                        {
                            p.x = metrics.innerQuad.left;
                            p.y = corners[ BottomLeftCorner ].centerY;

                            setCorner( BottomLeftCorner, metrics );
                        }
                        else
                        {
                            increment();
                            p.x = c.centerX - m_cos * c.radiusInnerX;
                            p.y = c.centerY - m_sin * c.radiusInnerY;

                            if ( p.y >= corners[ BottomLeftCorner ].centerY )
                                setCorner( BottomLeftCorner, metrics );
                        }

                        break;
                    }
                    case BottomLeftCorner:
                    {
                        if ( p.x >= c.centerX )
                        {
                            p.x = corners[ BottomRightCorner ].centerX;
                            p.y = metrics.innerQuad.bottom;

                            setCorner( BottomRightCorner, metrics );
                        }
                        else
                        {
                            increment();
                            p.x = c.centerX - m_cos * c.radiusInnerX;
                            p.y = c.centerY + m_sin * c.radiusInnerY;

                            if ( p.x >= corners[ BottomRightCorner ].centerX )
                                setCorner( BottomRightCorner, metrics );
                        }

                        break;
                    }
                    case BottomRightCorner:
                    {
                        increment();

                        p.x = c.centerX + m_cos * c.radiusInnerX;
                        p.y = c.centerY + m_sin * c.radiusInnerY;

                        break;
                    }
                    default:
                    {
                        Q_ASSERT( false );
                    }
                }
            }

            p.v = curve.valueAt( p.x, p.y );

            if ( ( p.v > 0.5 ) && ( p.v < m_contourLine.p1.v ) )
            {
                p = m_contourLine.p1;
                m_isDone = true;
            }
        }

      private:
        static constexpr qreal m_eps = 1e-4;

        inline void setCorner(
            Qt::Corner corner, const QskBoxRenderer::Metrics& metrics )
        {
            m_corner = corner;
            const auto& c = metrics.corner[ corner ];

            const double angleStep = M_PI_2 / c.stepCount;

            m_cosStep = qFastCos( angleStep );
            m_sinStep = qFastSin( angleStep );
            m_stepInv1 = m_sinStep / m_cosStep;
            m_stepInv2 = m_cosStep + m_sinStep * m_stepInv1;

            bool horizontal;
            if ( corner == Qt::TopRightCorner || corner == Qt::BottomLeftCorner )
                horizontal = !m_clockwise;
            else
                horizontal = m_clockwise;

            if ( horizontal )
            {
                m_cos = 1.0;
                m_sin = 0.0;

                m_sinStep = -m_sinStep;
            }
            else
            {
                m_cos = 0.0;
                m_sin = 1.0;
            }
        }

        inline void increment()
        {
#if 0
            /*
                We are running into this assertions when closing the filling
                at the bottom right corner for rectangles with small width/height
                ratio. Seems to be no problem, but has to be understood. TODO ...
             */

            Q_ASSERT( m_sinStep < 0.0 || m_cos < 1.0 );
            Q_ASSERT( m_sinStep > 0.0 || m_cos > 0.0 );
#endif

            const double cos0 = m_cos;

            m_cos = m_cos * m_cosStep + m_sin * m_sinStep;
            m_sin = m_sin * m_cosStep - cos0 * m_sinStep;

            if ( m_sinStep < 0.0 )
            {
                if ( m_cos < m_eps )
                    m_cos = 0.0;

                if ( m_sin > 1.0 - m_eps )
                    m_sin = 1.0;
            }
            else
            {
                if ( m_cos > 1.0 - m_eps )
                    m_cos = 1.0;

                if ( m_sin < m_eps )
                    m_sin = 0.0;
            }
        }

        inline void decrement()
        {
            m_cos = ( m_cos - m_stepInv1 * m_sin ) / m_stepInv2;
            if ( std::abs( m_cos ) < m_eps )
                m_cos = 0.0;

            m_sin = m_sin / m_cosStep + m_stepInv1 * m_cos;
            if ( std::abs( m_sin ) < m_eps )
                m_sin = 0.0;
        }

        bool m_clockwise;
        bool m_isLeading;
        bool m_isDone;

        qreal m_cos, m_cosStep;
        qreal m_sin, m_sinStep;
        qreal m_stepInv1, m_stepInv2;

        ContourLine m_contourLine;
        Qt::Corner m_corner;
    };

    class OutlineIterator
    {
      public:
        OutlineIterator( const QskBoxRenderer::Metrics& metrics,
                const ValueCurve& curve, bool clockwise )
            : m_metrics( metrics )
            , m_curve( curve )
        {
            const auto& c = metrics.corner[ Qt::TopLeftCorner ];

#if 1
            // This does not need to be done twice !!!
#endif
            const double angleStep = M_PI_2 / c.stepCount;
            const qreal cosStep = qFastCos( angleStep );
            const qreal sinStep = qFastSin( angleStep );

            /*
                Initialize the iterators to start with the
                minimal value, what is somewhere around the top left corner
                when having a gradient going from top/left to bottom/right.
             */
            qreal cos1 = 0.0;
            qreal sin1 = 1.0;

            qreal x1 = c.centerX;
            qreal y1 = metrics.innerQuad.top;
            qreal v1 = m_curve.valueAt( x1, y1 );

            for ( int step = 1;; step++ )
            {
                const qreal cos2 = cos1 * cosStep + sin1 * sinStep;
                const qreal sin2 = sin1 * cosStep - cos1 * sinStep;

                const qreal x2 = c.centerX - c.radiusInnerX * cos2;
                const qreal y2 = c.centerY - c.radiusInnerY * sin2;
                const qreal v2 = m_curve.valueAt( x2, y2 );

                if ( v2 >= v1 || step >= c.stepCount )
                {
                    if ( clockwise )
                    {
                        m_iterator[ 0 ].setup( metrics, true, true,
                            cos1, cosStep, sin1, sinStep, x2, y2, v2, x1, y1, v1 );

                        m_iterator[ 1 ].setup( metrics, false, false,
                            cos2, cosStep, sin2, sinStep, x1, y1, v1, x2, y2, v2 );
                    }
                    else
                    {
                        m_iterator[ 0 ].setup( metrics, true, false,
                            cos2, cosStep, sin2, sinStep, x1, y1, v1, x2, y2, v2 );

                        m_iterator[ 1 ].setup( metrics, false, true,
                            cos1, cosStep, sin1, sinStep, x2, y2, v2, x1, y1, v1 );
                    }

                    while ( !m_iterator[ 1 ].isDone() &&
                        ( m_iterator[ 0 ].value() > m_iterator[ 1 ].value() ) )
                    {
                        m_iterator[ 1 ].advance( metrics, m_curve );
                    }

                    return;
                }

                cos1 = cos2;
                sin1 = sin2;

                x1 = x2;
                y1 = y2;
                v1 = v2;
            }
        }

        inline void advance()
        {
            m_iterator[ 0 ].advance( m_metrics, m_curve );

            if ( !m_iterator[ 0 ].isDone() )
            {
                /*
                    adjusting the counter vertex until its top value is above
                    the value of the border. Then our value is between
                    the values of the counter vertex and we find out counter
                    border point by cutting the counter vertex.
                 */

                while ( !m_iterator[ 1 ].isDone() &&
                    ( m_iterator[ 0 ].value() > m_iterator[ 1 ].value() ) )
                {
                    m_iterator[ 1 ].advance( m_metrics, m_curve );
                }
            }
        }

        inline bool isDone() const
        {
            return m_iterator[ 0 ].isDone();
        }

        inline qreal value() const
        {
            return m_iterator[ 0 ].value();
        }

        inline void setLineAt( qreal value, Color color, ColoredLine* line )
        {
            const auto& contourLine1 = m_iterator[ 0 ].contourLine();
            const auto& contourLine2 = m_iterator[ 1 ].contourLine();

            const QPointF pos = contourLine1.pointAt( value );
            const QPointF cPos = contourLine2.pointAt( value );

            if ( m_iterator[ 0 ].isClockwise() )
                setLine( cPos.x(), cPos.y(), pos.x(), pos.y(), color, line );
            else
                setLine( pos.x(), pos.y(), cPos.x(), cPos.y(), color, line );
        }

        inline void setLine( Color color, ColoredLine* line )
        {
            const auto& contourLine1 = m_iterator[ 0 ].contourLine();
            const auto& contourLine2 = m_iterator[ 1 ].contourLine();

            const QPointF cPos = contourLine2.pointAt( contourLine1.p2.v );

            if ( m_iterator[ 0 ].isClockwise() )
                setLine( cPos.x(), cPos.y(), contourLine1.p2.x, contourLine1.p2.y, color, line );
            else
                setLine( contourLine1.p2.x, contourLine1.p2.y, cPos.x(), cPos.y(), color, line );
        }

      private:
        inline void setLine( qreal x1, qreal y1, qreal x2, qreal y2,
            Color color, ColoredLine* line )
        {
            line->setLine( x1, y1, x2, y2, color );
        }

        const QskBoxRenderer::Metrics& m_metrics;
        const ValueCurve& m_curve;

        /*
            The first iterator for running along the left or right
            half of the ellipse. The other one is for finding the
            corresponing point at the other side. */

        ContourIterator m_iterator[ 2 ];
    };

    class DRectellipseIterator
    {
      public:
        DRectellipseIterator(
                const QskBoxRenderer::Metrics& metrics, const ValueCurve& curve )
            : m_left( metrics, curve, false )
            , m_right( metrics, curve, true )
        {
            m_next = ( m_left.value() < m_right.value() ) ? &m_left : &m_right;
        }

        template< class ColorIterator >
        inline void setGradientLine( const ColorIterator& it, ColoredLine* line )
        {
            m_next->setLineAt( it.value(), it.color(), line );
        }

        template< class ColorIterator >
        inline void setContourLine( const ColorIterator& it, ColoredLine* line )
        {
            m_next->setLine( it.colorAt( m_next->value() ), line );
        }

        inline qreal value() const
        {
            return m_next->value();
        }

        inline bool advance()
        {
            m_next->advance();
            m_next = ( m_left.value() < m_right.value() ) ? &m_left : &m_right;

            return !m_next->isDone();
        }

      private:
        OutlineIterator m_left, m_right;
        OutlineIterator* m_next;
    };
}

void QskBoxRenderer::renderDiagonalFill( const QskBoxRenderer::Metrics& metrics,
    const QskGradient& gradient, int fillLineCount, QskVertex::ColoredLine* lines )
{
    const ValueCurve curve( metrics );

    DRectellipseIterator it( metrics, curve );
    auto line = QskVertex::fillOrdered( it, 0.0, 1.0, gradient, lines );

    /*
        There are a couple of reasons, why less points have been rendered
        than expected: f.e the positions of the gradient lines are
        calculated from a diagonal, where the start/endpoints
        might be a little outside of the contour.

        As effects like this one are hard to precalculate we simply
        insert dummy lines to match the allocated memory.
     */

    while ( line - lines < fillLineCount )
    {
        line[ 0 ] = line[ -1 ];
        line++;
    }
}
