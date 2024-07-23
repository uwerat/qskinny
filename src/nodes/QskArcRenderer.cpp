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
        OrthogonalStroker( const QRectF& rect, qreal thickness, qreal border )
            : m_thickness( thickness )
            , m_border( border )
            , m_rx( 0.5 * ( rect.width() - m_thickness ) )
            , m_ry( 0.5 * ( rect.height() - m_thickness ) )
            , m_offsetOut( 0.5 * m_thickness )
            , m_offsetIn( m_offsetOut - border )
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

            const auto x = m_cx + m_rx * cos;
            const auto y = m_cy - m_ry * sin;

            const auto v = normalVector( cos, sin ) * m_offsetIn;

            line.setLine( x + v.x(), y - v.y(), x - v.x(), y + v.y(), color );
        }

        inline void setBorderLines( const qreal radians,
            const QskVertex::Color color, QskVertex::ColoredLine& outer,
            QskVertex::ColoredLine& inner  ) const
        {
            const auto cos = qFastCos( radians );
            const auto sin = qFastSin( radians );

            const auto x = m_cx + m_rx * cos;
            const auto y = m_cy - m_ry * sin;

            const auto v = normalVector( cos, sin );

            const auto v1 = v * m_offsetOut;
            const auto v2 = v * m_offsetIn;

            outer.setLine( x + v1.x(), y - v1.y(), x + v2.x(), y - v2.y(), color );
            inner.setLine( x - v1.x(), y + v1.y(), x - v2.x(), y + v2.y(), color );
        }

        inline void setClosingBorderLines( const QSGGeometry::ColoredPoint2D& pos,
            QskVertex::ColoredLine* lines, qreal sign, const QskVertex::Color color ) const
        {
            const auto& l0 = lines[0];

            const auto dx = sign * l0.dy();
            const auto dy = sign * l0.dx();

            lines[-3].setLine( pos.x, pos.y, pos.x, pos.y, color );
            lines[-2].setLine( pos.x + dx, pos.y - dy, pos.x, pos.y, color );
            lines[-1].setLine( l0.x1() + dx, l0.y1() - dy, l0.x1(), l0.y1(), color );
        }

      private:

        inline QPointF normalVector( const qreal radians ) const
        {
            return normalVector( qFastCos( radians ), qFastSin( radians ) );
        }

        inline QPointF normalVector( const qreal cos, const qreal sin ) const
        {
            /*
                The inner/outer points are found by shifting orthogonally along the
                ellipse tangent:

                    m = w / h * tan( angle )
                    y = m * x;
                    x² + y² = 1.0

                    => x = 1.0 / sqrt( 1.0 + m² );

                Note: the angle of the orthogonal vector could
                      also be found ( first quadrant ) by:

                    atan2( tan( angle ), h / w );
             */

            if ( qFuzzyIsNull( cos ) )
                return { 0.0, ( sin > 0.0 ) ? 1.0 : -1.0 };

            const qreal m = m_aspectRatio * ( sin / cos );
            const qreal t = 1.0 / qSqrt( 1.0 + m * m );

            const auto dx = ( cos >= 0.0 ) ? t : -t;
            return { dx, m * dx };
        }

        const qreal m_thickness;
        const qreal m_border;

        // radii t the middle of the arc
        const qreal m_rx, m_ry;

        // distances between the middle and inner/outer of the border
        const qreal m_offsetOut, m_offsetIn;

        const qreal m_aspectRatio; // m_rx / m_ry

        // center
        const qreal m_cx, m_cy;
    };

    class RadialStroker
    {
      public:
        RadialStroker( const QRectF& rect, qreal thickness, qreal border )
            : m_sx( qMax( rect.width() / rect.height(), 1.0 ) )
            , m_sy( qMax( rect.height() / rect.width(), 1.0 ) )
            , m_rx1( 0.5 * rect.width() )
            , m_ry1( 0.5 * rect.height() )
            , m_rx2( m_rx1 - m_sx * border )
            , m_ry2( m_ry1 - m_sy * border )
            , m_rx3( m_rx1 - m_sx * ( thickness - border ) )
            , m_ry3( m_ry1 - m_sy * ( thickness - border ) )
            , m_rx4( m_rx1 - m_sx * thickness )
            , m_ry4( m_ry1 - m_sy * thickness )
            , m_cx( rect.x() + m_rx1 )
            , m_cy( rect.y() + m_ry1 )
        {
        }

        inline void setLine( const qreal radians,
            const QskVertex::Color color, QskVertex::ColoredLine& line  ) const
        {
            const auto cos = qFastCos( radians );
            const auto sin = qFastSin( radians );

            line.setLine( m_cx + m_rx2 * cos, m_cy - m_ry2 * sin,
                m_cx + m_rx3 * cos, m_cy - m_ry3 * sin, color );
        }

        inline void setBorderLines( const qreal radians,
            const QskVertex::Color color, QskVertex::ColoredLine& outer,
            QskVertex::ColoredLine& inner  ) const
        {
            const auto cos = qFastCos( radians );
            const auto sin = qFastSin( radians );

            outer.setLine( m_cx + m_rx1 * cos, m_cy - m_ry1 * sin,
                m_cx + m_rx2 * cos, m_cy - m_ry2 * sin, color );

            inner.setLine( m_cx + m_rx4 * cos, m_cy - m_ry4 * sin,
                m_cx + m_rx3 * cos, m_cy - m_ry3 * sin, color );
        }

        inline void setClosingBorderLines( const QSGGeometry::ColoredPoint2D& pos,
            QskVertex::ColoredLine* lines, qreal sign, const QskVertex::Color color ) const
        {
            // Good enough until it is decided if we want to keep the radial mode.
            const auto& l0 = lines[0];

            const auto s = m_sx / m_sy;
            const auto dx = sign * l0.dy() * s;
            const auto dy = sign * l0.dx() / s;

            lines[-3].setLine( pos.x, pos.y, pos.x, pos.y, color );
            lines[-2].setLine( pos.x + dx, pos.y - dy, pos.x, pos.y, color );
            lines[-1].setLine( l0.x1() + dx, l0.y1() - dy, l0.x1(), l0.y1(), color );
        }

      private:
        // stretch factors of the ellipse
        const qreal m_sx, m_sy;

        // radii: out->in
        const qreal m_rx1, m_ry1, m_rx2, m_ry2, m_rx3, m_ry3, m_rx4, m_ry4;

        // center point
        const qreal m_cx, m_cy;
    };
}

namespace
{
    class StrokerBase
    {
      protected:
        StrokerBase( const QRectF& rect, const QskArcMetrics& metrics, bool radial )
            : m_rect( rect )
            , m_radians1( qDegreesToRadians( metrics.startAngle() ) )
            , m_radians2( qDegreesToRadians( metrics.endAngle() ) )
            , m_radial( qFuzzyCompare( rect.width(), rect.height() ) ? true : radial )
            , m_closed( metrics.isClosed() )
        {
        }

        int arcLineCount() const
        {
            // not very sophisticated - TODO ...

            const auto radius = 0.5 * qMax( m_rect.width(), m_rect.height() );
            const auto radians = qAbs( m_radians2 - m_radians1 );

            const auto count = qCeil( ( radius * radians ) / 3.0 );
            return qBound( 3, count, 160 );
        }

        inline qreal radiansAt( qreal progress ) const
        {
            return m_radians1 + progress * ( m_radians2 - m_radians1 );
        }

        const QRectF& m_rect;

        const qreal m_radians1;
        const qreal m_radians2;

        const bool m_radial; // for circular arcs radial/orthogonal does not differ
        const bool m_closed;
    };
}

namespace
{
    class FillStroker : private StrokerBase
    {
      public:
        FillStroker( const QRectF&, const QskArcMetrics&,
            bool radial, const QskGradient& );

        int lineCount() const;
        int setLines( qreal thickness, qreal border, QskVertex::ColoredLine* ) const;

      private:
        template< class LineStroker >
        int renderLines( const LineStroker&, QskVertex::ColoredLine* ) const;

        const QskGradient& m_gradient;
    };

    FillStroker::FillStroker( const QRectF& rect,
            const QskArcMetrics& metrics, bool radial, const QskGradient& gradient )
        : StrokerBase( rect, metrics, radial )
        , m_gradient( gradient )
    {
    }

    int FillStroker::lineCount() const
    {
        return arcLineCount() + m_gradient.stepCount() - 1;
    }

    inline int FillStroker::setLines( const qreal thickness,
        const qreal border, QskVertex::ColoredLine* lines ) const
    {
        if ( m_radial )
        {
            const RadialStroker lineStroker( m_rect, thickness, border );
            return renderLines( lineStroker, lines );
        }
        else
        {
            const OrthogonalStroker lineStroker( m_rect, thickness, border );
            return renderLines( lineStroker, lines );
        }
    }

    template< class LineStroker >
    inline int FillStroker::renderLines(
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
}

namespace
{
    class BorderStroker : StrokerBase
    {
      public:
        BorderStroker( const QRectF&, const QskArcMetrics&,
            bool radial, const QColor& color );

        int lineCount() const;
        int setLines( qreal thickness, qreal border, QskVertex::ColoredLine* ) const;

      private:
        template< class LineStroker >
        int renderLines( const LineStroker&, QskVertex::ColoredLine* ) const;

        const QskVertex::Color m_color;
    };

    BorderStroker::BorderStroker( const QRectF& rect,
            const QskArcMetrics& metrics, bool radial, const QColor& color )
        : StrokerBase( rect, metrics, radial )
        , m_color( color )
    {
    }

    int BorderStroker::lineCount() const
    {
        auto count = 2 * arcLineCount();
        if ( !m_closed )
            count += 2 * 3;

        return count;
    }

    int BorderStroker::setLines( const qreal thickness, const qreal border,
        QskVertex::ColoredLine* lines ) const
    {
        if ( m_radial )
        {
            const RadialStroker stroker( m_rect, thickness, border );
            return renderLines( stroker, lines );
        }
        else
        {
            const OrthogonalStroker stroker( m_rect, thickness, border );
            return renderLines( stroker, lines );
        }
    }

    template< class LineStroker >
    inline int BorderStroker::renderLines( const LineStroker& stroker,
        QskVertex::ColoredLine* lines ) const
    {
        const auto count = arcLineCount();

        const qreal stepMax = count - 1;
        const auto stepSize = ( m_radians2 - m_radians1 ) / stepMax;

        auto outer = lines;
        if ( !m_closed )
            outer += 3;

        auto inner = outer + count;
        if ( !m_closed )
            inner += 3;

        for ( int i = 0; i < count; i++ )
        {
            stroker.setBorderLines( m_radians1 + i * stepSize,
                m_color, outer[i], inner[count - 1 - i] );
        }

        if ( !m_closed )
        {
            const auto sign = ( stepSize > 0.0 ) ? 1.0 : -1.0;

            stroker.setClosingBorderLines( inner[count - 1].p1, outer, sign, m_color );
            stroker.setClosingBorderLines( outer[count - 1].p1, inner, sign, m_color );
        }

        return 2 * ( count + ( m_closed ? 0 : 3 ) );
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

    FillStroker stroker( rect, metrics, radial, gradient );

    const auto lineCount = stroker.lineCount();

    const auto lines = qskAllocateColoredLines( geometry, lineCount );
    if ( lines == nullptr )
        return;

    const auto effectiveCount = stroker.setLines(
        metrics.thickness(), borderWidth, lines );

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
    Q_ASSERT( borderWidth > 0.0 );

    geometry.setDrawingMode( QSGGeometry::DrawTriangleStrip );

    BorderStroker stroker( rect, metrics, radial, borderColor );

    const auto lineCount = stroker.lineCount();

    const auto lines = qskAllocateColoredLines( geometry, lineCount );
    if ( lines )
    {
        const auto effectiveCount = stroker.setLines(
            metrics.thickness(), borderWidth, lines );

        if ( lineCount != effectiveCount )
        {
            qFatal( "geometry: allocated memory does not match: %d vs. %d",
                effectiveCount, lineCount );
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

