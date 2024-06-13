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
    class OrthogonalStroker
    {
      public:
        OrthogonalStroker( const QRectF& rect, qreal length )
            : m_l2( 0.5 * length )
            , m_rx( 0.5 * ( rect.width() - length ) )
            , m_ry( 0.5 * ( rect.height() - length ) )
            , m_aspectRatio( m_rx / m_ry )
            , m_cx( rect.x() + 0.5 * rect.width() )
            , m_cy( rect.y() + 0.5 * rect.height() )
        {
        }

        inline void setLine( const qreal radians,
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
                dy = ( sin > 0.0 ) ? m_l2 : -m_l2;
            }
            else
            {
                const qreal m = m_aspectRatio * ( sin / cos );
                const qreal t = m_l2 / qSqrt( 1.0 + m * m );

                dx = ( cos >= 0.0 ) ? t : -t;
                dy = m * dx;
            }

            const auto x = m_cx + m_rx * cos;
            const auto y = m_cy - m_ry * sin;

            line.setLine( x + dx, y - dy, x - dx, y + dy, color );
        }

        const qreal m_l2; // half of the length

        // radius
        const qreal m_rx;
        const qreal m_ry;
        const qreal m_aspectRatio; // m_rx / m_ry

        // center
        const qreal m_cx;
        const qreal m_cy;
    };

    class RadialStroker
    {
      public:
        RadialStroker( const QRectF& rect, qreal length )
            : m_length( length )
            , m_rx1( 0.5 * rect.width() )
            , m_ry1( 0.5 * rect.height() )
            , m_rx2( m_rx1 - ( ( m_rx1 > m_ry1 ) ? length * m_rx1 / m_ry1 : length ) )
            , m_ry2( m_ry1 - ( ( m_ry1 > m_rx1 ) ? length * m_ry1 / m_rx1 : length ) )
            , m_cx( rect.x() + m_rx1 )
            , m_cy( rect.y() + m_ry1 )
        {
        }

        inline void setLine( const qreal radians,
            const QskVertex::Color color, QskVertex::ColoredLine& line  ) const
        {
            const auto cos = qFastCos( radians );
            const auto sin = qFastSin( radians );

            line.setLine( m_cx + m_rx1 * cos, m_cy - m_ry1 * sin,
                m_cx + m_rx2 * cos, m_cy - m_ry2 * sin, color );
        }

      private:
        const qreal m_length;

        // outer radii
        const qreal m_rx1;
        const qreal m_ry1;

        // inner radii
        const qreal m_rx2;
        const qreal m_ry2;

        // center point
        const qreal m_cx;
        const qreal m_cy;
    };

    class ArcStroker
    {
      public:
        ArcStroker( const QRectF&, const QskArcMetrics&, bool radial, const QskGradient& );

        int fillCount() const;
        int borderCount() const;

        int setBorderLines( QskVertex::ColoredLine*, const QskVertex::Color ) const;
        int setFillLines( int length, QskVertex::ColoredLine* ) const;

      private:
        int arcLineCount() const;
        qreal radiansAt( qreal progress ) const;

        template< class LineStroker >
        int renderFillLines( const LineStroker&, QskVertex::ColoredLine* ) const;

        // radius
        const QRectF& m_rect;

        const qreal m_radians1;
        const qreal m_radians2;

        const bool m_radial; // for circular arcs radial/orthogonal does not differ

        const QskGradient& m_gradient;
    };

    ArcStroker::ArcStroker( const QRectF& rect,
            const QskArcMetrics& metrics, bool radial, const QskGradient& gradient )
        : m_rect( rect )
        , m_radians1( qDegreesToRadians( metrics.startAngle() ) )
        , m_radians2( qDegreesToRadians( metrics.endAngle() ) )
        , m_radial( qFuzzyCompare( rect.width(), rect.height() ) ? true : radial )
        , m_gradient( gradient )
    {
    }

    int ArcStroker::fillCount() const
    {
        return arcLineCount() + m_gradient.stepCount() - 1;
    }

    int ArcStroker::arcLineCount() const
    {
        // not very sophisticated - TODO ...

        const auto radius = 0.5 * qMax( m_rect.width(), m_rect.height() );
        const auto radians = qAbs( m_radians2 - m_radians1 );

        const auto count = qCeil( ( radius * radians ) / 3.0 );
        return qBound( 3, count, 160 );
    }

    int ArcStroker::borderCount() const
    {
        return 0;
    }

    int ArcStroker::setBorderLines( QskVertex::ColoredLine* lines,
        const QskVertex::Color color ) const
    {
        Q_UNUSED( lines );
        Q_UNUSED( color );

        return 0;
    }

    inline int ArcStroker::setFillLines(
        const int length, QskVertex::ColoredLine* lines ) const
    {
        if ( m_radial )
        {
            const RadialStroker lineStroker( m_rect, length );
            return renderFillLines( lineStroker, lines );
        }
        else
        {
            const OrthogonalStroker lineStroker( m_rect, length );
            return renderFillLines( lineStroker, lines );
        }
    }

    template< class LineStroker >
    inline int ArcStroker::renderFillLines(
        const LineStroker& stroker, QskVertex::ColoredLine* lines ) const
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

        const auto count = arcLineCount();

        const qreal stepMax = count - 1;
        const auto stepSize = ( m_radians2 - m_radians1 ) / stepMax;

        for ( int i = 0; i < count; i++ )
        {
            const auto progress = i / stepMax;

            while( !it.isDone() && ( it.position() < progress ) )
            {
                stroker.setLine( radiansAt( it.position() ), it.color(), *l++ );
                it.advance();
            }

            const auto color = it.colorAt( progress );
            stroker.setLine( m_radians1 + i * stepSize, color, *l++ );
        }

        return l - lines;
    }

    inline qreal ArcStroker::radiansAt( qreal progress ) const
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
    const QskArcMetrics& metrics, bool radial, qreal borderWidth,
    const QskGradient& gradient, QSGGeometry& geometry )
{
    geometry.setDrawingMode( QSGGeometry::DrawTriangleStrip );

    const auto b2 = 0.5 * borderWidth;
    const auto r = rect.adjusted( b2, b2, -b2, -b2 );

    ArcStroker stroker( r, metrics, radial, gradient );

    const auto lineCount = stroker.fillCount();

    const auto lines = qskAllocateColoredLines( geometry, lineCount );
    if ( lines == nullptr )
        return;

    const auto effectiveCount = stroker.setFillLines(
        metrics.thickness() - borderWidth, lines );

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

void QskArcRenderer::renderBorder( const QRectF& rect, const QskArcMetrics& metrics,
    bool radial, qreal borderWidth, const QColor& borderColor, QSGGeometry& geometry )
{
    Q_UNUSED( borderWidth );
    Q_UNUSED( radial );

    geometry.setDrawingMode( QSGGeometry::DrawTriangleStrip );

    ArcStroker stroker( rect, metrics, radial, QskGradient() );

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

void QskArcRenderer::renderBorderGeometry( const QRectF& rect,
    const QskArcMetrics& metrics, bool radial, qreal borderWidth, QSGGeometry& geometry )
{
    Q_UNUSED( rect );
    Q_UNUSED( metrics );
    Q_UNUSED( radial );
    Q_UNUSED( borderWidth );
    Q_UNUSED( geometry );
}

void QskArcRenderer::renderFillGeometry( const QRectF& rect,
    const QskArcMetrics& metrics, bool radial, qreal borderWidth, QSGGeometry& geometry )
{
    Q_UNUSED( rect );
    Q_UNUSED( metrics );
    Q_UNUSED( radial );
    Q_UNUSED( borderWidth );
    Q_UNUSED( geometry );
}

