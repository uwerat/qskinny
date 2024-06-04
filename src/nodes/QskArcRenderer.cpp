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
        qreal radiansAt( qreal progress ) const;

        void setFillLine( const qreal radians,
            const QskVertex::Color, QskVertex::ColoredLine& ) const;

        const qreal m_extent;

        // radius
        const qreal m_rx;
        const qreal m_ry;

        // center
        const qreal m_cx;
        const qreal m_cy;

        const qreal m_radians1;
        const qreal m_radians2;


        const QskGradient& m_gradient;
    };

    EllipseStroker::EllipseStroker( const QRectF& rect,
            const QskArcMetrics& m, qreal borderWidth, const QskGradient& gradient )
        : m_extent( 0.5 * ( m.thickness() - borderWidth ) )
        , m_rx( 0.5 * rect.width() - m_extent )
        , m_ry( 0.5 * rect.height() - m_extent )
        , m_cx( rect.center().x() )
        , m_cy( rect.center().y() )
        , m_radians1( qDegreesToRadians( m.startAngle() ) )
        , m_radians2( qDegreesToRadians( m.endAngle() ) )
        , m_gradient( gradient )
    {
    }

    int EllipseStroker::fillCount() const
    {
        return arcLineCount() + m_gradient.stepCount() - 1;
    }

    int EllipseStroker::arcLineCount() const
    {
        // not very sophisticated - TODO ...

        const auto radius = qMax( m_rx, m_ry ) + m_extent; // outer border
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

    inline void EllipseStroker::setFillLine( const qreal radians,
        const QskVertex::Color color, QskVertex::ColoredLine& line  ) const
    {
        const auto cos = qFastCos( radians );
        const auto sin = qFastSin( radians );

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

        if ( qFuzzyIsNull( cos ) )
        {
            dx = 0.0;
            dy = ( sin > 0.0 ) ? m_extent : -m_extent;
        }
        else
        {
            const qreal m = ( m_rx / m_ry ) * ( sin / cos );
            const qreal t = m_extent / qSqrt( 1.0 + m * m );

            dx = ( cos >= 0.0 ) ? t : -t;
            dy = m * dx;
        }

        const auto x = m_cx + m_rx * cos;
        const auto y = m_cy - m_ry * sin;

        line.setLine( x + dx, y - dy, x - dx, y + dy, color );
    }

    int EllipseStroker::setFillLines( QskVertex::ColoredLine* lines ) const
    {
        auto l = lines;

        QskBoxRenderer::GradientIterator it;
        if ( m_gradient.stepCount() <= 1 )
        {
            it.reset( m_gradient.rgbStart(), m_gradient.rgbEnd() );
        }
        else
        {
            it.reset( m_gradient.stops() );
            it.advance(); // the first stop is always covered by the contour
        }

        const auto stepCount = arcLineCount();
        const auto stepSize = ( m_radians2 - m_radians1 ) / ( stepCount - 1 );

        for ( int i = 0; i < stepCount; i++ )
        {
            const auto progress = qreal( i ) / stepCount;

            for ( ; it.position() < progress; it.advance() )
                setFillLine( radiansAt( it.position() ), it.color(), *l++ );

            const auto color = it.colorAt( progress );
            setFillLine( m_radians1 + i * stepSize, color, *l++ );
        }

        return l - lines;
    }

    inline qreal EllipseStroker::radiansAt( qreal progress ) const
    {
        return m_radians1 + progress * ( m_radians2 - m_radians1 );
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
             */

            for ( int i = effectiveCount; i < lineCount; i++ )
                lines[i] = lines[i - 1];
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
