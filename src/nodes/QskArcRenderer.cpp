/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskArcRenderer.h"
#include "QskArcMetrics.h"
#include "QskGradient.h"
#include "QskVertex.h"

#include <qsggeometry.h>

#if 1
#include <qdebug.h>
#endif

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

static inline int qskApproximatedCircumference( const QRectF& rect )
{
    const qreal a = rect.width();
    const qreal b = rect.height();

    const auto ratio = a / b;
    if ( ratio > 0.9 || ratio < 1.1 )
        return std::max( a, b ) * 2.0 * M_PI; // circle

    // Srinivasa Ramanujan: https://en.wikipedia.org/wiki/Ellipse#Circumference

    const qreal d1 = ( a - b );
    const qreal d2 = ( a + b );
    const qreal h = ( d1 * d1 ) / ( d2 * d2 );

    return M_PI * d2 * ( 1.0 + ( 3 * h / ( 10 + sqrt( 4 - 3 * h ) ) ) );
}

static inline int qskStepCount( const QRectF& rect )
{
#if 0
    const auto dist = 3.0;
#else
    const auto dist = 20.0;
#endif

    const int length = qskApproximatedCircumference( rect );
    return std::max( 3, qCeil( length / dist ) );
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
        inline bool isDone() const { return m_stepIndex > m_stepCount; }

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
        , m_radiansStep( ( radians2 - radians1 ) / stepCount )
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

        int setBorderLines( QskVertex::ColoredLine* ) const;

      private:
        int arcLineCount() const;

        void setArcLines( QskVertex::ColoredLine*, int lineCount,
            const QPointF&, const qreal width, const qreal height,
            const qreal radians1, const qreal radians2,
            qreal arcWidth, const QskVertex::Color ) const;

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
        return 0; // TODO
    }

    int Stroker::arcLineCount() const
    {
        if ( m_metrics.isNull() )
            return 0;

        int n = qskStepCount( m_rect );
        if ( !m_metrics.isClosed() )
            n = qCeil( n * qAbs( m_metrics.spanAngle() ) / 360.0 );

        return n;
    }

    int Stroker::borderCount() const
    {
        if ( m_metrics.isNull() )
            return 0;

        return 2 * arcLineCount() + 1;
    }

    void Stroker::setArcLines( QskVertex::ColoredLine* lines, int lineCount,
        const QPointF& center, const qreal w, const qreal h,
        const qreal radians1, const qreal radians2,
        qreal arcWidth, const QskVertex::Color color ) const
    {
        const auto w2 = w - arcWidth;
        const auto h2 = h - arcWidth;

        auto l = lines;

        for ( AngleIterator it( radians1, radians2, lineCount - 1 ); !it.isDone(); ++it )
        {
            const auto x1 = center.x() + w * it.cos();
            const auto x2 = center.x() + w2 * it.cos();

            const auto y1 = center.y() + h * it.sin();
            const auto y2 = center.y() + h2 * it.sin();

            l++->setLine( x1, y1, x2, y2, color );
        }

        if ( l - lines != lineCount )
            qWarning() << lineCount << "->" << l - lines;
        Q_ASSERT( l - lines == lineCount );
    }

    int Stroker::setBorderLines( QskVertex::ColoredLine* lines ) const
    {
        const auto center = m_rect.center();

        const qreal radians1 = qDegreesToRadians( m_metrics.startAngle() );
        const qreal radians2 = qDegreesToRadians( m_metrics.endAngle() );

        const int n = arcLineCount();

        const QskVertex::Color color( QColor( Qt::darkBlue ) );

        auto w = 0.5 * m_rect.width();
        auto h = 0.5 * m_rect.height();

        setArcLines( lines, n, center, w, h, radians1, radians2, m_borderWidth, color );

        w -= m_metrics.thickness() - m_borderWidth;
        h -= m_metrics.thickness() - m_borderWidth;

        setArcLines( lines + n + 1, n, center, w, h, radians2, radians1, m_borderWidth, color );

        lines[n] = { lines[n - 1].p2, lines[n + 1].p1 };

        return 2 * n + 1;
    }
}

void QskArcRenderer::renderBorderGeometry( const QRectF& rect,
    const QskArcMetrics& metrics, qreal borderWidth, QSGGeometry& geometry )
{
    geometry.setDrawingMode( QSGGeometry::DrawTriangleStrip );

    Stroker stroker( rect, metrics, borderWidth );

    const auto lineCount = stroker.borderCount();

    const auto lines = qskAllocateColoredLines( geometry, lineCount );
    if ( lines )
    {
        const auto effectiveCount = stroker.setBorderLines( lines );
        if ( lineCount != effectiveCount )
        {
            qWarning() << lineCount << effectiveCount;
        }
    }
}

void QskArcRenderer::renderFillGeometry( const QRectF& rect,
    const QskArcMetrics& metrics, qreal borderWidth, QSGGeometry& geometry )
{
    Q_UNUSED( rect );
    Q_UNUSED( geometry );

    Stroker stroker( rect, metrics, borderWidth );

    const auto lines = qskAllocateColoredLines( geometry, stroker.fillCount() );
    if ( lines )
    {
        // TODO
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
