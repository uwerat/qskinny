/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskArcRenderer.h"
#include "QskArcMetrics.h"
#include "QskGradient.h"
#include "QskVertex.h"

#include <qsggeometry.h>
#include <qdebug.h>

static inline QskVertex::Line* qskAllocateLines(
    QSGGeometry& geometry, int lineCount )
{
    geometry.allocate( 2 * lineCount ); // 2 points per line
    return reinterpret_cast< QskVertex::Line* >( geometry.vertexData() );
}

static inline QskVertex::ColoredLine* qskAllocateColoredLines(
    QSGGeometry& geometry, int lineCount )
{
    geometry.allocate( 2 * lineCount ); // 2 points per line
    return reinterpret_cast< QskVertex::ColoredLine* >( geometry.vertexData() );
}

namespace
{
    class AngleIterator
    {
      public:
        AngleIterator( qreal radians1, qreal radians2, int stepCount );

        inline void operator++() { increment(); }
        void increment();

        inline double cos() const { return m_cos; }
        inline double sin() const { return m_sin; }

        inline int step() const { return m_stepIndex; }
        inline int stepCount() const { return m_stepCount; }
        inline bool isDone() const { return m_stepIndex >= m_stepCount; }

      private:
        double m_cos;
        double m_sin;

        int m_stepIndex;
        int m_stepCount;

        const double m_radians1;
        const double m_radians2;
        const double m_radiansStep;
    };

    inline AngleIterator::AngleIterator( qreal radians1, qreal radians2, int stepCount )
        : m_stepIndex( 0 )
        , m_stepCount( stepCount )
        , m_radians1( radians1 )
        , m_radians2( radians2 )
        , m_radiansStep( ( radians2 - radians1 ) / ( stepCount - 1 ) )
    {
        m_cos = qFastCos( radians1 );
        m_sin = qFastSin( radians1 );
    }

    inline void AngleIterator::increment()
    {
        if ( ++m_stepIndex >= m_stepCount )
        {
            if ( m_stepIndex == m_stepCount )
            {
                m_cos = qFastCos( m_radians2 );
                m_sin = qFastSin( m_radians2 );
            }
        }
        else
        {
            const auto radians = m_radians1 + m_stepIndex * m_radiansStep;

            m_cos = qFastCos( radians );
            m_sin = qFastSin( radians );
        }
    }
}

namespace
{
    class Stroker
    {
      public:
        Stroker( const QRectF&, const QskArcMetrics& metrics, qreal borderWidth );

        int fillCount() const;
        int borderCount() const;

        int setBorderLines( QskVertex::ColoredLine*, const QskVertex::Color ) const;
        int setFillLines( QskVertex::ColoredLine*, const QskVertex::Color ) const;

      private:
        int arcLineCount( qreal radians = 2.0 * M_PI ) const;
        QLineF fillLineAt( qreal x, qreal y, qreal sin, qreal cos ) const;

        const QRectF& m_rect;
        const QskArcMetrics& m_metrics;
        const qreal m_borderWidth;
    };

    Stroker::Stroker( const QRectF& rect,
            const QskArcMetrics& metrics, qreal borderWidth )
        : m_rect( rect )
        , m_metrics( metrics )
        , m_borderWidth( borderWidth )
    {
        Q_ASSERT( metrics.sizeMode() == Qt::AbsoluteSize );
    }

    int Stroker::fillCount() const
    {
        int n = 0;

        qreal radians1 = qDegreesToRadians( m_metrics.startAngle() );
        qreal radians2 = qDegreesToRadians( m_metrics.endAngle() );

        if ( radians2 < radians1 )
            qSwap( radians1, radians2 );

        for ( auto r = qFloor( radians1 / M_PI_2 ) * M_PI_2;
            r < radians2; r += M_PI_2 )
        {
            const auto r1 = qMax( r, radians1 );
            const auto r2 = qMin( r + M_PI_2, radians2 );

            n += arcLineCount( r2 - r1 );
        }

        return n;
    }

    int Stroker::arcLineCount( const qreal radians ) const
    {
        // not very sophisticated - TODO ...

        const auto ratio = qAbs( radians ) / ( 2.0 * M_PI );

        int n = ( m_rect.width() + m_rect.height() ) * M_PI_2;
        n = qBound( 3, n, 80 );
        n = qCeil( n * ratio  );

        return n;
    }

    int Stroker::borderCount() const
    {
        if ( m_metrics.isNull() )
            return 0;

        return 0;
    }

    int Stroker::setBorderLines( QskVertex::ColoredLine* lines,
        const QskVertex::Color color ) const
    {
        Q_UNUSED( lines );
        Q_UNUSED( color );

        return 0;
    }

    int Stroker::setFillLines( QskVertex::ColoredLine* lines,
        const QskVertex::Color color ) const
    {
        qreal radians1 = qDegreesToRadians( m_metrics.startAngle() );
        qreal radians2 = qDegreesToRadians( m_metrics.endAngle() );

        if ( m_metrics.spanAngle() < 0.0 )
            std::swap( radians1, radians2 );

        const qreal w = 0.5 * ( m_rect.width() - m_metrics.thickness() );
        const qreal h = 0.5 * ( m_rect.height() - m_metrics.thickness() );

        const auto center = m_rect.center();

        auto l = lines;

        for ( auto r = qFloor( radians1 / M_PI_2 ) * M_PI_2;
            r < radians2; r += M_PI_2 )
        {
            const auto r1 = qMax( r, radians1 );
            const auto r2 = qMin( r + M_PI_2, radians2 );

            const auto lineCount = arcLineCount( r2 - r1 );

            for ( AngleIterator it( r1, r2, lineCount ); !it.isDone(); ++it )
            {
                const auto line = fillLineAt( w, h, it.cos(), it.sin() );

                l++->setLine( center.x() + line.x1(), center.y() - line.y1(),
                    center.x() + line.x2(), center.y() - line.y2(), color );
            }
        }

        return l - lines;
    }

    inline qreal sqr( qreal x )
    {
        return x * x;
    }

    QLineF Stroker::fillLineAt( qreal w, qreal h, qreal cos, qreal sin ) const
    {
        const auto x = w * cos;
        const auto y = h * sin;

        /*
            The inner/outer points are found by shifting along the
            normal vector of the tangent at the ellipse point.
         */

        const auto t = 0.5 * m_metrics.thickness();

        if ( qFuzzyIsNull( sin ) )
        {
            const qreal dx = cos * t;
            return QLineF( x + dx, y, x - dx, y );
        }
        else if ( qFuzzyIsNull( cos ) )
        {
            const qreal dy = sin * t;
            return QLineF( x, y + dy, x, y - dy );
        }

        const qreal m = qSqrt( w * w - x * x ) * ( w / h ) / x;
        const auto dt = t * qSqrt( ( 1.0 / ( 1.0 + m * m ) ) ); 

        const qreal dx = ( x >= 0 ) ? dt : -dt;
        const qreal dy = m * ( ( y >= 0 ) ? dx : -dx );

        const auto x1 = x + dx;
        const auto y1 = y + dy;

        const auto x2 = x - dx;
        const auto y2 = y - dy;

        return QLineF( x1, y1, x2, y2 );
    }
}

void QskArcRenderer::renderFillGeometry( const QRectF& rect,
    const QskArcMetrics& metrics, qreal borderWidth, QSGGeometry& geometry )
{
    geometry.setDrawingMode( QSGGeometry::DrawTriangleStrip );

    Stroker stroker( rect, metrics, borderWidth );

    const auto lineCount = stroker.fillCount();

    const auto lines = qskAllocateColoredLines( geometry, lineCount );
    if ( lines )
    {
        const auto effectiveCount = stroker.setFillLines( lines, QColor( Qt::darkRed ) );
        if ( lineCount != effectiveCount )
        {
            qWarning() << lineCount << effectiveCount;
        }
    }
}

void QskArcRenderer::renderFillGeometry( const QRectF& rect,
    const QskArcMetrics& metrics, QSGGeometry& geometry )
{
    renderFillGeometry( rect, metrics, 0.0, geometry );
}

bool QskArcRenderer::isGradientSupported(
    const QskArcMetrics& metrics, const QskGradient& gradient )
{
    if ( metrics.isNull() || !gradient.isVisible() || gradient.isMonochrome() )
        return true;

    return gradient.type() == QskGradient::Stops;
}

void QskArcRenderer::renderArc( const QRectF& rect,
    const QskArcMetrics& metrics, qreal borderWidth, const QColor& borderColor,
    const QskGradient& gradient, QSGGeometry& geometry )
{
    Q_UNUSED( rect );
    Q_UNUSED( metrics );
    Q_UNUSED( borderWidth );
    Q_UNUSED( borderColor );
    Q_UNUSED( gradient );
    Q_UNUSED( geometry );
}

void QskArcRenderer::renderArc( const QRectF& rect, const QskArcMetrics& metrics,
    const QskGradient& gradient, QSGGeometry& geometry )
{
    renderArc( rect, metrics, 0, QColor(), gradient, geometry );
}

void QskArcRenderer::renderBorder( const QRectF& rect, const QskArcMetrics& metrics,
    qreal borderWidth, const QColor& borderColor, QSGGeometry& geometry )
{
    geometry.setDrawingMode( QSGGeometry::DrawTriangleStrip );

    Stroker stroker( rect, metrics, borderWidth );

    const auto lineCount = stroker.borderCount();

    const auto lines = qskAllocateColoredLines( geometry, lineCount );
    if ( lines )
    {
        const auto effectiveCount = stroker.setBorderLines( lines, borderColor );
        if ( lineCount != effectiveCount )
        {
            qWarning() << lineCount << effectiveCount;
        }
    }
}
