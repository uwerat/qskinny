/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxGradientStroker.h"
#include "QskBoxBasicStroker.h"
#include "QskVertex.h"
#include "QskBoxColorMap.h"
#include "QskBoxMetrics.h"

static inline bool qskCanUseHVFiller(
    const Qt::Orientations orientations, const QskLinearDirection& dir )
{
    if ( !dir.isTilted() )
        return orientations & ( dir.isVertical() ? Qt::Vertical : Qt::Horizontal );

    return false;
}

namespace
{
    using namespace QskVertex;

    class Value
    {
      public:
        qreal from, to; // opposite to the direction of the gradient
        qreal pos;      // in direction of the gradient
    };

    class FillerHV
    {
      public:
        FillerHV( const QskBoxMetrics& metrics )
            : m_metrics( metrics )
        {
        }

        int setLines( const QskGradient& gradient, ColoredLine* lines )
        {
            using namespace Qt;

            const auto corners = m_metrics.corners;
            const auto dir = gradient.linearDirection();

            m_isVertical = dir.isVertical();
            m_t0 = m_isVertical ? dir.y1() : dir.x1();
            m_dt = m_isVertical ? dir.dy() : dir.dx();

            ColoredLine* l = lines;
            Value v1, v2;
            ArcIterator arcIt;

            m_gradientIterator.reset( gradient.stops() );

            m_c1 = &corners[ Qt::TopLeftCorner ];
            m_c2 = &corners[ m_isVertical ? Qt::TopRightCorner : Qt::BottomLeftCorner ];
            m_c3 = ( m_c1->stepCount > m_c2->stepCount ) ? m_c1 : m_c2;

            arcIt.reset( m_c3->stepCount, !m_isVertical );

            v2 = contourValue( arcIt );

            skipGradientLines( v2.pos );
            setContourLine( v2, l++ );

            arcIt.increment();

            do
            {
                v1 = v2;
                v2 = contourValue( arcIt );

                l = setGradientLines( v1, v2, l );
                setContourLine( v2, l++ );

                arcIt.increment();
            } while( !arcIt.isDone() );

            m_c1 = &corners[ m_isVertical ? Qt::BottomLeftCorner : Qt::TopRightCorner ];
            m_c2 = &corners[ Qt::BottomRightCorner ];
            m_c3 = ( m_c1->stepCount > m_c2->stepCount ) ? m_c1 : m_c2;

            arcIt.reset( m_c3->stepCount, m_isVertical );

            if ( contourValue( arcIt ).pos <= v2.pos )
                arcIt.increment(); // ellipse: opening/closing parts are connected

            do
            {
                v1 = v2;
                v2 = contourValue( arcIt );

                l = setGradientLines( v1, v2, l );
                setContourLine( v2, l++ );

                arcIt.increment();

            } while( !arcIt.isDone() );

            return l - lines;
        }

        inline void skipGradientLines( qreal pos )
        {
            while ( !m_gradientIterator.isDone() )
            {
                if ( m_t0 + m_gradientIterator.position() * m_dt > pos )
                    return;

                m_gradientIterator.advance();
            }
        }

        inline ColoredLine* setGradientLines(
            const Value& v1, const Value& v2, ColoredLine* lines )
        {
            while ( !m_gradientIterator.isDone() )
            {
                const auto pos = m_t0 + m_gradientIterator.position() * m_dt;

                if ( pos > v2.pos || qFuzzyIsNull( v2.pos - pos ) )
                    return lines;

                const auto color = m_gradientIterator.color();

                const qreal f = ( pos - v1.pos ) / ( v2.pos - v1.pos );

                const qreal t1 = v1.from + f * ( v2.from - v1.from );
                const qreal t2 = v1.to + f * ( v2.to - v1.to );

                setLine( t1, t2, pos, color, lines++ );

                m_gradientIterator.advance();
            }

            return lines;
        }

        inline void setContourLine( const Value& v, ColoredLine* line )
        {
            const auto color = m_gradientIterator.colorAt( ( v.pos - m_t0 ) / m_dt );
            setLine( v.from, v.to, v.pos, color, line );
        }

      private:
        inline Value contourValue( const ArcIterator& arcIt ) const
        {
            const auto cos = arcIt.cos();
            const auto sin = arcIt.sin();

            if ( m_isVertical )
                return { m_c1->xInner( cos ), m_c2->xInner( cos ), m_c3->yInner( sin ) };
            else
                return { m_c1->yInner( sin ), m_c2->yInner( sin ), m_c3->xInner( cos ) };
        }

        inline void setLine( qreal from, qreal to, qreal pos,
            Color color, ColoredLine* line )
        {
            if ( m_isVertical )
                line->setLine( from, pos, to, pos, color );
            else
                line->setLine( pos, from, pos, to, color );
        }

        const QskBoxMetrics& m_metrics;

        bool m_isVertical;
        qreal m_t0, m_dt;

        const QskBoxMetrics::Corner* m_c1, * m_c2, * m_c3;
        QskBox::GradientIterator m_gradientIterator;
    };
}

namespace
{
    using namespace QskVertex;

    class Point
    {
      public:
        Point() = default;
        inline Point( qreal x, qreal y, qreal v ): x( x ), y( y ), v( v ) {}

        qreal x = 0;
        qreal y = 0;
        qreal v = 0; // value at (x,y) according the gradient vector in the range [0,1]
    };

    /*
        Iterating over the interpolating ( ~tangent ) lines of the outline in order
        of increasing gradient values. We have the options to run clockwise or
        counter-clockwise
     */
    class TangentLineIterator
    {
      public:
        TangentLineIterator( const QskBoxMetrics& metrics,
            const QskLinearDirection& dir, bool isClockwise )
            : m_metrics( metrics )
            , m_dir( dir )
            , m_isClockwise( isClockwise )
        {
            using namespace Qt;

            if ( dir.dx() >= 0.0 )
            {
                if ( dir.dy() >= 0.0 )
                {
                    m_corners[0] = TopLeftCorner;
                    m_corners[1] = m_isClockwise ? TopRightCorner : BottomLeftCorner;
                    m_corners[2] = BottomRightCorner;
                }
                else
                {
                    m_corners[0] = BottomLeftCorner;
                    m_corners[1] = m_isClockwise ? BottomRightCorner : TopLeftCorner;
                    m_corners[2] = TopRightCorner;
                }
            }
            else
            {
                if ( dir.dy() >= 0.0 )
                {
                    m_corners[0] = TopRightCorner;
                    m_corners[1] = m_isClockwise ? TopLeftCorner : BottomRightCorner;
                    m_corners[2] = BottomLeftCorner;
                }
                else
                {
                    m_corners[0] = BottomRightCorner;
                    m_corners[1] = m_isClockwise ? BottomLeftCorner : TopRightCorner;
                    m_corners[2] = TopLeftCorner;
                }
            }
        }

        inline QPointF pointAt( qreal value ) const
        {
            const auto dv = m_p2.v - m_p1.v;
            if ( dv == 0.0 )
                return QPointF( m_p1.x, m_p1.y );

            const qreal r = ( value - m_p1.v ) / dv;
            return QPointF( m_p1.x + r * ( m_p2.x - m_p1.x ),
                m_p1.y + r * ( m_p2.y - m_p1.y ) );
        }

        void setup( const QskVertex::ArcIterator& arcIterator )
        {
            m_arcIterator = arcIterator;

            const auto& c = m_metrics.corners[ m_corners[0] ];

            const qreal x = c.xInner( m_arcIterator.cos() );
            const qreal y = c.yInner( m_arcIterator.sin() );

            m_p2 = m_p1 = { x, y, m_dir.valueAt( x, y ) };
        }

        inline bool isDone() const { return m_isDone; }

        inline qreal value() const { return m_p2.v; }
        inline Qt::Corner corner() const { return m_corners[ m_cornerIndex ]; }

        inline Point p1() const { return m_p1; }
        inline Point p2() const { return m_p2; }

        void incrementTo( qreal value )
        {
            while ( !isDone() && ( value > m_p2.v ) )
                advance();
        }

        inline void advance()
        {
            if ( m_isDone )
                return;

            m_arcIterator.increment();

            auto c = &m_metrics.corners[ m_corners[ m_cornerIndex ] ];

            if( m_arcIterator.isDone() )
            {
                if ( m_cornerIndex < 2 )
                {
                    c = &m_metrics.corners[ m_corners[ ++m_cornerIndex ] ];
                    m_arcIterator.reset( c->innerStepCount(), !m_arcIterator.isInverted() );
                }
            }

            m_p1 = m_p2;

            m_p2.x = c->xInner( m_arcIterator.cos() );
            m_p2.y = c->yInner( m_arcIterator.sin() );

            m_p2.v = m_dir.valueAt( m_p2.x, m_p2.y );

            if ( m_cornerIndex == 2 )
            {
                if ( ( m_p2.v < m_p1.v ) || m_arcIterator.isDone() )
                {
                    // passing the opposite position from our starting point
                    m_p2 = m_p1;
                    m_isDone = true;
                }
            }
        }

      private:
        const QskBoxMetrics& m_metrics;
        const QskLinearDirection m_dir;

        const bool m_isClockwise;
        bool m_isDone = false;

        QskVertex::ArcIterator m_arcIterator;

        Point m_p1, m_p2;

        int m_cornerIndex = 0;
        Qt::Corner m_corners[3];
    };

    /*
        OutlineIterator iterates a pair of tangent lines in direction of the gradient vector.
        The second tangent line increases in oppsite direction so that
        the value of the leading point of the first tangent line is in between the values
        of the opposite tangent line.

        Contour and gradient lines can then be found from cutting the tangent lines
        with the perpendicular of the gradient vector.
     */
    class OutlineIterator
    {
      public:
        OutlineIterator( const QskBoxMetrics& metrics,
            const QskLinearDirection& dir, bool clockwise )
            : m_isVertical( dir.dx() == 0.0 )
            , m_it1( metrics, dir, clockwise )
            , m_it2( metrics, dir, !clockwise )
        {
            const auto corner = m_it1.corner();
            const auto& c = metrics.corners[ m_it1.corner() ];

            bool inverted = clockwise;
            if ( corner == Qt::TopLeftCorner || corner == Qt::BottomRightCorner )
                inverted = clockwise;
            else
                inverted = !clockwise;

            QskVertex::ArcIterator arcIt;
            arcIt.reset( c.innerStepCount(), inverted );

            if ( c.innerStepCount() == 1 )
            {
                m_it1.setup( arcIt );
                m_it2.setup( arcIt.reverted() );

                // not rounded
                m_it1.advance();
                m_it2.advance();
            }
            else
            {
                arcIt.reset( c.innerStepCount(), clockwise );

                qreal v1 = dir.valueAt( c.xInner( arcIt.cos() ), c.yInner( arcIt.sin() ) );

                do
                {
                    arcIt.increment();

                    const qreal v2 = dir.valueAt(
                        c.xInner( arcIt.cos() ), c.yInner( arcIt.sin() ) );

                    if ( v2 > v1 )
                        break;

                    v1 = v2;

                } while( !arcIt.isDone() );

                arcIt.decrement();

                m_it1.setup( arcIt );
                m_it2.setup( arcIt.reverted() );
            }

            m_it2.incrementTo( m_it1.value() );
        }

        inline void advance()
        {
            m_it1.advance();

            /*
                Increasing the opposite iterator until its value becomes larger
                than the value of the leading iterator. Then the opposite
                point for the next line can be found by interpolating
                between p1/p2 of the opposite iterator.
             */

            if ( !m_it1.isDone() )
                m_it2.incrementTo( m_it1.value() );
        }

        inline bool isDone() const { return m_it1.isDone(); }
        inline qreal value() const { return m_it1.value(); }

        inline void setLineAt( qreal value, Color color, ColoredLine* line )
        {
            const auto p1 = m_it1.pointAt( value );
            const auto p2 = m_it2.pointAt( value );

            setLine( p1.x(), p1.y(), p2.x(), p2.y(), color, line );
        }

        inline void setLine( Color color, ColoredLine* line )
        {
            const auto p1 = m_it1.p2();
            const auto p2 = m_it2.pointAt( p1.v );

            setLine( p1.x, p1.y, p2.x(), p2.y(), color, line );
        }

      private:
        inline void setLine( qreal x1, qreal y1, qreal x2, qreal y2,
            Color color, ColoredLine* line )
        {
            if ( m_isVertical )
            {
                if ( x1 < x2 )
                    line->setLine( x1, y1, x2, y2, color );
                else
                    line->setLine( x2, y2, x1, y1, color );
            }
            else
            {
                if ( y1 < y2 )
                    line->setLine( x1, y1, x2, y2, color );
                else
                    line->setLine( x2, y2, x1, y1, color );
            }
        }

        /*
            The first iterator for running along the left or right
            half of the ellipse. The other one is for finding the
            corresponing point at the other side.
         */

        const bool m_isVertical;
        TangentLineIterator m_it1, m_it2;
    };

    /*
        ContourIterator runs a pair of OutlineIterators, one clockwise the other
        counter clockwise in direction of the gradient vector, so that we
        always pass all corners.

        The next contour line can always be found from the outline iterator with the
        larger gradient value.  Gradient lines can then be found from interpolating
        between the tangent lines of the outline iterators.
     */
    class ContourIterator
    {
      public:
        ContourIterator(
            const QskBoxMetrics& metrics, const QskLinearDirection& dir )
            : m_left( metrics, dir, false )
            , m_right( metrics, dir, true )
        {
            m_right.advance();
            m_next = &m_left;
        }

        inline void setGradientLine( qreal value, Color color, ColoredLine* line )
        {
            m_next->setLineAt( value, color, line );
        }

        inline void setContourLine( Color color, ColoredLine* line )
        {
            m_next->setLine( color, line );
        }

        inline qreal value() const { return m_next->value(); }

        inline bool advance()
        {
            if ( qFuzzyIsNull( m_left.value() - m_right.value() ) )
            {
                m_left.advance();
                m_right.advance();
            }
            else
            {
                m_next->advance();
            }

            if ( m_next->isDone() )
                return false;

            m_next = ( m_left.value() < m_right.value() ) ? &m_left : &m_right;

            return true;
        }

      private:
        OutlineIterator m_left, m_right;
        OutlineIterator* m_next;
    };

    class FillerD
    {
      public:
        FillerD( const QskBoxMetrics& metrics )
            : m_metrics( metrics )
        {
        }

        int setLines( const QskGradient& gradient, ColoredLine* lines )
        {
            ContourIterator it( m_metrics, gradient.linearDirection() );
            QskBox::GradientIterator gradientIt( gradient.stops() );

            ColoredLine* l = lines;

            // skip leading gradient lines
            while ( !gradientIt.isDone() )
            {
                const auto pos = gradientIt.position();
                if ( pos > it.value() || qFuzzyIsNull( pos - it.value() ) )
                    break;

                gradientIt.advance();
            }

            do
            {
                // gradient lines
                while ( !gradientIt.isDone() )
                {
                    const auto pos = gradientIt.position();
                    if ( pos > it.value() || qFuzzyIsNull( pos - it.value() ) )
                        break;

                    it.setGradientLine( pos, gradientIt.color(), l++ );

                    gradientIt.advance();
                }

                // contour line
                const auto color = gradientIt.colorAt( it.value() );
                it.setContourLine( color, l++ );

            } while ( it.advance() );

            return l - lines;
        }

      private:
        const QskBoxMetrics& m_metrics;
    };

    class FillerRect
    {
      public:
        FillerRect( const QskBoxMetrics& metrics )
            : m_metrics( metrics )
        {
        }

        int setLines( const QskGradient& gradient, ColoredLine* lines ) const
        {
            const qreal x1 = m_metrics.innerRect.left();
            const qreal x2 = m_metrics.innerRect.right();
            const qreal y1 = m_metrics.innerRect.top();
            const qreal y2 = m_metrics.innerRect.bottom();

            QskBox::GradientIterator it( gradient.stops() );
            ColoredLine* l = lines;

            const auto dir = gradient.linearDirection();

            if ( dir.isTilted() )
            {
                const qreal m = dir.dy() / dir.dx();
                const auto vec = dir.vector();

                struct { qreal x, y, value; } c1, c2, c3, c4;

                {
                    // corners sorted in order their values
                    c1 = { x1, y1, dir.valueAt( x1, y1 ) };
                    c2 = { x2, y1, dir.valueAt( x2, y1 ) };
                    c3 = { x1, y2, dir.valueAt( x1, y2 ) };
                    c4 = { x2, y2, dir.valueAt( x2, y2 ) };

                    if ( m < 0.0 )
                    {
                        qSwap( c1, c3 );
                        qSwap( c2, c4 );
                    }

                    if ( c1.value > c4.value )
                        qSwap( c1, c4 );

                    if ( c2.value > c3.value )
                        qSwap( c2, c3 );
                }

                // skipping all gradient lines before the first corner
                while ( !it.isDone() && ( it.position() <= c1.value ) )
                    it.advance();

                setLine( c1.x, c1.y, c1.x, c1.y, it.colorAt( c1.value ), l++ );

                while ( !it.isDone() && ( it.position() < c2.value ) )
                {
                    const auto p = vec.pointAt( it.position() );

                    const qreal y1 = p.y() + ( p.x() - c1.x ) / m;
                    const qreal x2 = p.x() + ( p.y() - c1.y ) * m;

                    setLine( c1.x, y1, x2, c1.y, it.color(), l++ );
                    it.advance();
                }

                if ( c1.x == c3.x ) // cutting left/right edges
                {
                    const auto dy = ( c2.x - c3.x ) / m;

                    setLine( c2.x, c2.y, c3.x, c2.y + dy, it.colorAt( c2.value ), l++ );

                    while ( !it.isDone() && ( it.position() < c3.value ) )
                    {
                        const auto p = vec.pointAt( it.position() );

                        const qreal y1 = p.y() + ( p.x() - c2.x ) / m;
                        const qreal y2 = p.y() + ( p.x() - c3.x ) / m;

                        setLine( c2.x, y1, c3.x, y2, it.color(), l++ );
                        it.advance();
                    }

                    setLine( c2.x, c3.y - dy, c3.x, c3.y, it.colorAt( c3.value ), l++ );
                }
                else // cutting top/bottom edges
                {
                    const qreal dx = ( c2.y - c3.y ) * m;

                    setLine( c2.x, c2.y, c2.x + dx, c3.y, it.colorAt( c2.value ), l++ );

                    while ( !it.isDone() && ( it.position() < c3.value ) )
                    {
                        const auto p = vec.pointAt( it.position() );

                        const qreal x1 = p.x() + ( p.y() - c2.y ) * m;
                        const qreal x2 = p.x() + ( p.y() - c3.y ) * m;

                        setLine( x1, c2.y, x2, c3.y, it.color(), l++ );
                        it.advance();
                    }

                    setLine( c3.x - dx, c2.y, c3.x, c3.y, it.colorAt( c3.value ), l++ );
                }

                while ( !it.isDone() && ( it.position() < c4.value ) )
                {
                    const auto p = vec.pointAt( it.position() );

                    const qreal y1 = p.y() + ( p.x() - c4.x ) / m;
                    const qreal x2 = p.x() + ( p.y() - c4.y ) * m;

                    setLine( c4.x, y1, x2, c4.y, it.color(), l++ );
                    it.advance();
                }

                setLine( c4.x, c4.y, c4.x, c4.y, it.colorAt( c4.value ), l++ );
            }
            else if ( dir.isVertical() )
            {
                Q_ASSERT( dir.dy() > 0.0 ); // normalized in QskBoxRenderer

                const qreal min = ( y1 - dir.y1() ) / dir.dy();
                const qreal max = ( y2 - dir.y1() ) / dir.dy();

                while ( !it.isDone() && ( it.position() <= min ) )
                    it.advance();

                setHLine( y1, it.colorAt( min ), l++ );

                while ( !it.isDone() && ( it.position() < max ) )
                {
                    const auto y = dir.y1() + it.position() * dir.dy();
                    setHLine( y, it.color(), l++ );

                    it.advance();
                }

                setHLine( y2, it.colorAt( max ), l++ );
            }
            else // dir.isHorizontal
            {
                Q_ASSERT( dir.dx() > 0.0 ); // normalized in QskBoxRenderer

                const qreal min = ( x1 - dir.x1() ) / dir.dx();
                const qreal max = ( x2 - dir.x1() ) / dir.dx();

                while ( !it.isDone() && ( it.position() <= min ) )
                    it.advance();

                setVLine( x1, it.colorAt( min ), l++ );

                while ( !it.isDone() && ( it.position() < max ) )
                {
                    const auto x = dir.x1() + it.position() * dir.dx();
                    setVLine( x, it.color(), l++ );

                    it.advance();
                }

                setVLine( x2, it.colorAt( max ), l++ );
            }

            return l - lines;
        }

      private:

        inline void setHLine( qreal y,
            QskVertex::Color color, QskVertex::ColoredLine* line ) const
        {
            const auto& r = m_metrics.innerRect;
            line->setLine( r.left(), y, r.right(), y, color );
        }

        inline void setVLine( qreal x,
            QskVertex::Color color, QskVertex::ColoredLine* line ) const
        {
            const auto& r = m_metrics.innerRect;
            line->setLine( x, r.top(), x, r.bottom(), color );
        }

        inline void setLine( qreal x1, qreal y1, qreal x2, qreal y2,
            Color color, ColoredLine* line ) const
        {
            if ( x1 <= x2 )
                line->setLine( x1, y1, x2, y2, color );
            else
                line->setLine( x2, y2, x1, y1, color );
        }

        const QskBoxMetrics& m_metrics;
    };
}

QskBoxGradientStroker::QskBoxGradientStroker(
        const QskBoxMetrics& metrics, const QskGradient& gradient )
    : m_metrics( metrics )
    , m_gradient( gradient )
    , m_dir( m_gradient.linearDirection() )
{

}

int QskBoxGradientStroker::lineCount() const
{
    if ( m_metrics.innerRect.isEmpty() || !m_gradient.isVisible() )
        return 0;

    int n = m_gradient.stepCount() - 1;

    if ( m_metrics.isInsideRounded )
    {
        if ( qskCanUseHVFiller( m_metrics.stepSymmetries, m_dir ) )
        {
            const QskBoxBasicStroker stroker( m_metrics, QskBoxBorderColors(), m_gradient );
            n += stroker.fillCount();
        }
        else
        {
            n += m_metrics.innerStepCount() + 4;
        }
    }
    else
    {
        n += 2;

        if ( m_dir.isTilted() )
            n += 2; // contour lines for the opposite corners
    }

    /*
        The gradient starts and/or ends inside of the rectangle
        and we have to add pad with extra gradient lines.
     */
    if ( !m_dir.contains( m_metrics.innerRect ) )
        n += 2;

    return n;
}

void QskBoxGradientStroker::setLines( int lineCount, QskVertex::ColoredLine* lines )
{
    int effectiveCount;

    if ( m_metrics.isInsideRounded )
    {
        if ( qskCanUseHVFiller( m_metrics.stepSymmetries, m_dir ) )
        {
            FillerHV filler( m_metrics );
            effectiveCount = filler.setLines( m_gradient, lines );
        }
        else
        {
            FillerD filler( m_metrics );
            effectiveCount = filler.setLines( m_gradient, lines );
        }
    }
    else // rectangle
    {
        FillerRect filler( m_metrics );
        effectiveCount = filler.setLines( m_gradient, lines );
    }

#if 0
    qDebug() << "expected:" << lineCount << ", got:" << effectiveCount;
#endif

    if ( lineCount > 0 )
    {
        if ( effectiveCount > lineCount )
        {
            qFatal( "geometry: allocated memory exceeded: %d vs. %d",
                effectiveCount, lineCount );
        }

        if ( effectiveCount < lineCount )
        {
            /*
                Gradient or contour lines might be at the same position
                and we end up with less lines, than expected.
                As it is hard to precalculate all corner cases we allow a defensive
                allocaton policy and simply fill up the line buffer with duplicates
                of the last line.
             */

            for ( int i = effectiveCount; i < lineCount; i++ )
                lines[i] = lines[i-1];
        }
    }
}
