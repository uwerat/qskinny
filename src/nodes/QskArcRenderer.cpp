/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskArcRenderer.h"
#include "QskArcMetrics.h"
#include "QskGradient.h"
#include "QskVertex.h"
#include "QskBoxColorMap.h"

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
    /*
        QskVertex::ArcIterator is slightly more efficient as it increments
        cos/sin instead of doing the table lookups that are behind qFastCos/qFastSin.
        For the moment we go with qFastCos/qFastSin, maybe later: TODO ...
     */
    class AngleIterator
    {
      public:
        AngleIterator( qreal radians1, qreal radians2, int stepCount );

        inline void operator++() { increment(); }
        void increment();

        inline qreal cos() const { return m_cos; }
        inline qreal sin() const { return m_sin; }

        inline int step() const { return m_stepIndex; }
        inline int stepCount() const { return m_stepCount; }
        inline bool isDone() const { return m_stepIndex >= m_stepCount; }

        inline qreal progress() const { return qreal( m_stepIndex ) / m_stepCount; }

      private:
        qreal m_cos;
        qreal m_sin;

        int m_stepIndex;
        int m_stepCount;

        const qreal m_radians1;
        const qreal m_radians2;
        const qreal m_radiansStep;
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
    class EllipseStroker
    {
      public:
        EllipseStroker( const QRectF&, const QskArcMetrics&,
            qreal borderWidth, const QskGradient& );

        int fillCount() const;
        int borderCount() const;

        int setBorderLines( QskVertex::ColoredLine*, const QskVertex::Color ) const;
        int setFillLines( QskVertex::ColoredLine* ) const;

      private:
        int arcLineCount() const;

        // radius
        const qreal m_rx;
        const qreal m_ry;

        // center
        const qreal m_cx;
        const qreal m_cy;

        const qreal m_radians1;
        const qreal m_radians2;

        const qreal m_extent;

        const QskGradient& m_gradient;
    };

    EllipseStroker::EllipseStroker( const QRectF& rect,
            const QskArcMetrics& m, qreal borderWidth, const QskGradient& gradient )
        : m_rx( 0.5 * rect.width() )
        , m_ry( 0.5 * rect.height() )
        , m_cx( rect.x() + m_rx )
        , m_cy( rect.y() + m_ry )
        , m_radians1( qDegreesToRadians( m.startAngle() ) )
        , m_radians2( qDegreesToRadians( m.endAngle() ) )
        , m_extent( 0.5 * ( m.thickness() - borderWidth ) )
        , m_gradient( gradient )
    {
    }

    int EllipseStroker::fillCount() const
    {
        return arcLineCount();
    }

    int EllipseStroker::arcLineCount() const
    {
        // not very sophisticated - TODO ...

        const auto radius = qMax( m_rx, m_ry );
        const auto radians = qAbs( m_radians2 - m_radians1 );

        const auto count = qCeil( ( radius * radians ) / 3.0 );
        return qBound( 3, count, 160 );
    }

    int EllipseStroker::borderCount() const
    {
        return 0;
    }

    int EllipseStroker::setBorderLines( QskVertex::ColoredLine* lines,
        const QskVertex::Color color ) const
    {
        Q_UNUSED( lines );
        Q_UNUSED( color );

        return 0;
    }

    int EllipseStroker::setFillLines( QskVertex::ColoredLine* lines ) const
    {
        const auto w = m_rx - m_extent;
        const auto h = m_ry - m_extent;
        const auto aspectRatio = w / h;

        auto l = lines;

        const QskVertex::Color color1 = m_gradient.rgbStart();
        const QskVertex::Color color2 = m_gradient.rgbEnd();

        for ( AngleIterator it( m_radians1, m_radians2, arcLineCount() ); !it.isDone(); ++it )
        {
            /*
                The inner/outer points are found by shifting orthogonally along the
                ellipse tangent:

                    m = w / h * tan( angle )
                    y = m * x;
                    x² + y² = dt 

                    => x = dt / sqrt( 1.0 + m² );

                Note: the angle of the orthogonal vector could
                      also be found ( first quadrant ) by:

                    atan2( tan( angle ), h / w );
             */

            qreal dx, dy;

            if ( qFuzzyIsNull( it.cos() ) )
            {
                dx = 0.0;
                dy = ( it.sin() > 0.0 ) ? m_extent : -m_extent;
            }
            else
            {
                const qreal m = aspectRatio * ( it.sin() / it.cos() );
                const qreal t = m_extent / qSqrt( 1.0 + m * m );

                dx = ( it.cos() >= 0.0 ) ? t : -t;
                dy = m * dx;
            }

            const auto x = m_cx + w * it.cos();
            const auto y = m_cy - h * it.sin();

            auto color = color1;
            if ( color1 != color2 )
                color = color.interpolatedTo( color2, it.progress() );

            l++->setLine( x + dx, y - dy, x - dx, y + dy, color );
        }

        return l - lines;
    }
}

bool QskArcRenderer::isGradientSupported( const QskGradient& gradient )
{
    if ( gradient.isVisible() && !gradient.isMonochrome() )
        return gradient.type() == QskGradient::Stops;

    return true;
}

void QskArcRenderer::renderFillGeometry( const QRectF& rect,
    const QskArcMetrics& metrics, qreal borderWidth,
    const QskGradient& gradient, QSGGeometry& geometry )
{
    geometry.setDrawingMode( QSGGeometry::DrawTriangleStrip );

    EllipseStroker stroker( rect, metrics, borderWidth, gradient );

    const auto lineCount = stroker.fillCount();

    const auto lines = qskAllocateColoredLines( geometry, lineCount );
    if ( lines )
    {
        const auto effectiveCount = stroker.setFillLines( lines );
        if ( lineCount != effectiveCount )
        {
            qWarning() << lineCount << effectiveCount;
        }
    }
}

void QskArcRenderer::renderBorder( const QRectF& rect, const QskArcMetrics& metrics,
    qreal borderWidth, const QColor& borderColor, QSGGeometry& geometry )
{
    geometry.setDrawingMode( QSGGeometry::DrawTriangleStrip );

    EllipseStroker stroker( rect, metrics, borderWidth, QskGradient() );

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
