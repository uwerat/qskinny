/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskArcRenderer.h"
#include "QskArcMetrics.h"
#include "QskGradient.h"
#include "QskVertex.h"
#include "QskBoxColorMap.h"
#include "QskRgbValue.h"

#include <qsggeometry.h>

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
    template< class Line >
    class OrthogonalStroker
    {
      public:
        OrthogonalStroker( const QRectF& rect, qreal thickness, qreal border )
            : m_thickness( thickness )
            , m_border( border )
            , m_rx( 0.5 * ( rect.width() - m_thickness ) )
            , m_ry( 0.5 * ( rect.height() - m_thickness ) )
            , m_offsetToBorder( 0.5 * m_thickness - border )
            , m_aspectRatio( m_rx / m_ry )
            , m_cx( rect.x() + 0.5 * rect.width() )
            , m_cy( rect.y() + 0.5 * rect.height() )
        {
        }

        inline void setLinesAt( const qreal radians,
            const QskVertex::Color fillColor, const QskVertex::Color borderColor,
            Line* fill, Line* outerBorder, Line* innerBorder  ) const
        {
            const auto cos = qFastCos( radians );
            const auto sin = qFastSin( radians );

            const auto v = normalVector( cos, sin );

            const QPointF p0( m_cx + m_rx * cos, m_cy - m_ry * sin );

            const auto v1 = v * m_offsetToBorder;

            const auto p1 = p0 + v1;
            const auto p2 = p0 - v1;

            if ( fill )
                fill->setLine( p1, p2, fillColor );

            if ( outerBorder )
            {
                const auto v2 = v * m_border;

                outerBorder->setLine( p1 + v2, p1, borderColor );
                innerBorder->setLine( p2 - v2, p2, borderColor );
            }
        }

        inline void setClosingBorderLines( const Line& l,
            Line* lines, qreal sign, const QskVertex::Color color ) const
        {
            const auto& pos = l.p1;
            const auto& l0 = lines[0];

            const auto dx = sign * l0.dy();
            const auto dy = sign * l0.dx();

            lines[-3].setLine( pos.x, pos.y, pos.x, pos.y, color );
            lines[-2].setLine( pos.x + dx, pos.y - dy, pos.x, pos.y, color );
            lines[-1].setLine( l0.x1() + dx, l0.y1() - dy, l0.x1(), l0.y1(), color );
        }

      private:
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

                Note: we return the vector mirrored vertically, so that it
                      matches the coordinate system used by Qt.
             */

            if ( qFuzzyIsNull( cos ) )
                return { 0.0, ( sin < 0.0 ) ? 1.0 : -1.0 };

            const qreal m = m_aspectRatio * ( sin / cos );
            const qreal t = 1.0 / qSqrt( 1.0 + m * m );

            const auto dx = ( cos >= 0.0 ) ? t : -t;
            return { dx, -m * dx };
        }

        const qreal m_thickness;
        const qreal m_border;

        // radii t the middle of the arc
        const qreal m_rx, m_ry;

        // distances between the middle and the beginning of the border
        const qreal m_offsetToBorder;

        const qreal m_aspectRatio; // m_rx / m_ry

        // center
        const qreal m_cx, m_cy;
    };

    template< class Line >
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
            , m_center( rect.x() + m_rx1, rect.y() + m_ry1 )
        {
        }

        inline void setLinesAt( const qreal radians,
            const QskVertex::Color fillColor, const QskVertex::Color borderColor,
            Line* fill, Line* outer, Line* inner  ) const
        {
            const QPointF v( qFastCos( radians ), -qFastSin( radians ) );

            const auto x1 = m_center.x() + m_rx2 * v.x();
            const auto y1 = m_center.y() + m_ry2 * v.y();

            const auto x2 = m_center.x() + m_rx3 * v.x();
            const auto y2 = m_center.y() + m_ry3 * v.y();

            if ( fill )
                fill->setLine( x1, y1, x2, y2, fillColor );

            if ( outer )
            {
                const auto x3 = m_center.x() + m_rx1 * v.x();
                const auto y3 = m_center.y() + m_ry1 * v.y();

                const auto x4 = m_center.x() + m_rx4 * v.x();
                const auto y4 = m_center.y() + m_ry4 * v.y();

                outer->setLine( x3, y3, x1, y1, borderColor );
                inner->setLine( x4, y4, x2, y2, borderColor );
            }
        }

        inline void setClosingBorderLines( const Line& l,
            Line* lines, qreal sign, const QskVertex::Color color ) const
        {
            const auto& pos = l.p1;

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
        const QPointF m_center;
    };

    template< class Line >
    class CircularStroker
    {
      public:
        CircularStroker( const QRectF& rect, qreal thickness, qreal border )
            : m_center( rect.center() )
            , m_radius( 0.5 * ( rect.width() - thickness ) )
            , m_distOut( 0.5 * thickness )
            , m_distIn( m_distOut - border )
        {
        }

        inline void setLinesAt( const qreal radians,
            const QskVertex::Color fillColor, const QskVertex::Color borderColor,
            Line* fill, Line* outer, Line* inner  ) const
        {
            const QPointF v( qFastCos( radians ), -qFastSin( radians ) );

            const auto p0 = m_center + m_radius * v;
            const auto dv1 = v * m_distIn;

            const auto p1 = p0 + dv1;
            const auto p2 = p0 - dv1;

            if ( fill )
                fill->setLine( p1, p2, fillColor );

            if ( outer )
            {
                const auto dv2 = v * m_distOut;

                const auto p3 = p0 + dv2;
                const auto p4 = p0 - dv2;

                outer->setLine( p3, p1, borderColor );
                inner->setLine( p4, p2, borderColor );
            }
        }

        inline void setClosingBorderLines( const Line& l,
            Line* lines, qreal sign, const QskVertex::Color color ) const
        {
            const auto& pos = l.p1;
            const auto& l0 = lines[0];

            const auto dx = sign * l0.dy();
            const auto dy = sign * l0.dx();

            lines[-3].setLine( pos.x, pos.y, pos.x, pos.y, color );
            lines[-2].setLine( pos.x + dx, pos.y - dy, pos.x, pos.y, color );
            lines[-1].setLine( l0.x1() + dx, l0.y1() - dy, l0.x1(), l0.y1(), color );
        }

      private:
        // center point
        const QPointF m_center;
        const qreal m_radius; // middle of the arc

        // distances from the middle to the inner/outer side of the border
        const qreal m_distOut, m_distIn;
    };
}

namespace
{
    class Renderer
    {
      public:
        Renderer( const QRectF&, const QskArcMetrics&,
            bool radial, const QskGradient&, const QskVertex::Color& );

        int fillCount() const;
        int borderCount() const;

        template< class Line >
        void renderArc( const qreal thickness, const qreal border, Line*, Line* ) const;

      private:
        int arcLineCount() const;

        template< class LineStroker, class Line >
        void renderLines( const LineStroker&, Line*, Line* ) const;

        const QRectF& m_rect;

        const qreal m_radians1;
        const qreal m_radians2;

        const bool m_radial; // for circular arcs radial/orthogonal does not differ
        const bool m_closed;

        const QskGradient& m_gradient;
        const QskVertex::Color m_borderColor;
    };

    Renderer::Renderer( const QRectF& rect, const QskArcMetrics& metrics,
            bool radial, const QskGradient& gradient, const QskVertex::Color& borderColor )
        : m_rect( rect )
        , m_radians1( qDegreesToRadians( metrics.startAngle() ) )
        , m_radians2( qDegreesToRadians( metrics.endAngle() ) )
        , m_radial( radial )
        , m_closed( metrics.isClosed() )
        , m_gradient( gradient )
        , m_borderColor( borderColor )
    {
    }

    int Renderer::arcLineCount() const
    {
        // not very sophisticated - TODO ...

        const auto radius = 0.5 * qMax( m_rect.width(), m_rect.height() );
        const auto radians = qAbs( m_radians2 - m_radians1 );

        const auto count = qCeil( ( radius * radians ) / 3.0 );
        return qBound( 3, count, 160 );
    }

    int Renderer::fillCount() const
    {
        if ( !m_gradient.isVisible() )
            return 0;

        return arcLineCount() + m_gradient.stepCount() - 1;
    }

    template< class Line >
    void Renderer::renderArc( const qreal thickness, const qreal border,
        Line* fillLines, Line* borderLines ) const
    {
        if ( qskFuzzyCompare( m_rect.width(), m_rect.height() ) )
        {
            const CircularStroker< Line > stroker( m_rect, thickness, border );
            renderLines( stroker, fillLines, borderLines );
        }
        else if ( m_radial )
        {
            const RadialStroker< Line > stroker( m_rect, thickness, border );
            renderLines( stroker, fillLines, borderLines );
        }
        else
        {
            const OrthogonalStroker< Line > stroker( m_rect, thickness, border );
            renderLines( stroker, fillLines, borderLines );
        }
    }

    template< class LineStroker, class Line >
    void Renderer::renderLines( const LineStroker& lineStroker,
        Line* fillLines, Line* borderLines ) const
    {
        QskBoxRenderer::GradientIterator it;

        if ( fillLines )
        {
            if ( m_gradient.stepCount() <= 1 )
            {
                it.reset( m_gradient.rgbStart(), m_gradient.rgbEnd() );
            }
            else
            {
                it.reset( m_gradient.stops() );
                it.advance(); // the first stop is always covered by the contour
            }
        }

        const auto count = arcLineCount();
        const auto radiansSpan = m_radians2 - m_radians1;

        const qreal stepMax = count - 1;
        const auto stepSize = radiansSpan / stepMax;

        auto l = fillLines;

        auto outer = borderLines;
        auto inner = borderLines;

        if ( borderLines )
        {
            outer = borderLines;
            if ( !m_closed )
                outer += 3;

            inner = outer + count;
            if ( !m_closed )
                inner += 3;
        }

        for ( int i = 0; i < count; i++ )
        {
            const auto progress = i / stepMax;

            while( !it.isDone() && ( it.position() < progress ) )
            {
                const auto radians = m_radians1 + it.position() * radiansSpan;
                lineStroker.setLinesAt( radians, it.color(), m_borderColor,
                    l++, nullptr, nullptr );

                it.advance();
            }

            const auto radians = m_radians1 + i * stepSize;
            const auto color = it.colorAt( progress );

            lineStroker.setLinesAt( radians, color, m_borderColor,
                l ? l++ : nullptr,
                outer ? outer + i : nullptr,
                inner ? inner + count - 1 - i : nullptr
            );
        }

        if ( borderLines && !m_closed )
        {
            const auto sign = ( radiansSpan > 0.0 ) ? 1.0 : -1.0;

            lineStroker.setClosingBorderLines( inner[count - 1], outer, sign, m_borderColor );
            lineStroker.setClosingBorderLines( outer[count - 1], inner, sign, m_borderColor );
        }
    }

    int Renderer::borderCount() const
    {
        if ( m_borderColor.a == 0 )
            return 0;

        auto count = 2 * arcLineCount();
        if ( !m_closed )
            count += 2 * 3;

        return count;
    }
}

bool QskArcRenderer::isGradientSupported( const QRectF& rect,
    const QskArcMetrics& metrics, const QskGradient& gradient )
{
    if ( rect.isEmpty() || metrics.isNull() )
        return true;

    if ( !gradient.isVisible() || gradient.isMonochrome() )
        return true;

    switch( gradient.type() )
    {
        case QskGradient::Stops:
        {
            return true;
        }
        case QskGradient::Conic:
        {
            const auto direction = gradient.conicDirection();
            if ( direction.center() == rect.center() )
            {
                const auto aspectRatio = rect.width() / rect.height();
                if ( qskFuzzyCompare( direction.aspectRatio(), aspectRatio ) )
                {
                    /*
                       we should be able to create a list of stops from
                       this gradient that works for the renderer. TODO ...
                     */
                }
            }

            return false;
        }
        default:
        {
            return false;
        }
    }

    return false;
}

void QskArcRenderer::setColoredBorderLines( const QRectF& rect,
    const QskArcMetrics& metrics, bool radial, qreal borderWidth,
    const QColor& borderColor, QSGGeometry& geometry )
{
    geometry.setDrawingMode( QSGGeometry::DrawTriangleStrip );
    geometry.markVertexDataDirty();

    if ( borderWidth <= 0.0 || !( borderColor.isValid() && borderColor.alpha() > 0 ) )
    {
        qskAllocateColoredLines( geometry, 0 );
        return;
    }

    const Renderer renderer( rect, metrics, radial, QskGradient(), borderColor );

    if ( const auto lines = qskAllocateColoredLines( geometry, renderer.borderCount() ) )
    {
        renderer.renderArc( metrics.thickness(), borderWidth,
            static_cast< QskVertex::ColoredLine* >( nullptr ), lines );
    }
}

void QskArcRenderer::setColoredFillLines( const QRectF& rect, const QskArcMetrics& metrics,
    bool radial, qreal borderWidth, const QskGradient& gradient, QSGGeometry& geometry )
{
    geometry.setDrawingMode( QSGGeometry::DrawTriangleStrip );
    geometry.markVertexDataDirty();

    if ( !gradient.isVisible() )
    {
        qskAllocateColoredLines( geometry, 0 );
        return;
    }

    const Renderer renderer( rect, metrics, radial, gradient, QColor( 0, 0, 0, 0 ) );

    if ( const auto lines = qskAllocateColoredLines( geometry, renderer.fillCount() ) )
    {
        renderer.renderArc( metrics.thickness(), borderWidth, lines,
            static_cast< QskVertex::ColoredLine* >( nullptr ) );
    }
}

void QskArcRenderer::setColoredBorderAndFillLines( const QRectF& rect,
    const QskArcMetrics& metrics, bool radial, qreal borderWidth,
    const QColor& borderColor,  const QskGradient& gradient, QSGGeometry& geometry )
{
    geometry.setDrawingMode( QSGGeometry::DrawTriangleStrip );
    geometry.markVertexDataDirty();

    const Renderer renderer( rect, metrics, radial, gradient,
        borderColor.isValid() ? borderColor : QColor( 0, 0, 0, 0 ) );

    const auto borderCount = renderer.borderCount();
    const auto fillCount = renderer.fillCount();

    auto lineCount = borderCount + fillCount;
    if ( borderCount && fillCount )
        lineCount++; // connecting line

    const auto lines = qskAllocateColoredLines( geometry, lineCount );
    if ( lines )
    {
        const auto fillLines = fillCount ? lines : nullptr;
        const auto borderLines = borderCount ? lines + lineCount - borderCount : nullptr;

        renderer.renderArc( metrics.thickness(), borderWidth, fillLines, borderLines );

        if ( fillCount && borderCount )
        {
            const auto idx = fillCount;

            lines[idx].p1 = lines[idx - 1].p2;
            lines[idx].p2 = lines[idx + 1].p1;
        }
    }
}

void QskArcRenderer::setBorderLines( const QRectF& rect,
    const QskArcMetrics& metrics, bool radial, qreal borderWidth, QSGGeometry& geometry )
{
    geometry.setDrawingMode( QSGGeometry::DrawTriangleStrip );
    geometry.markVertexDataDirty();

    if ( borderWidth <= 0.0 )
    {
        qskAllocateLines( geometry, 0 );
        return;
    }

    const Renderer renderer( rect, metrics, radial, QskGradient(), 0 );

    const auto lines = qskAllocateLines( geometry, renderer.borderCount() );
    if ( lines )
    {
        QskVertex::Line* fill = nullptr;
        renderer.renderArc( metrics.thickness(), borderWidth, fill, lines );
    }
}

void QskArcRenderer::setFillLines( const QRectF& rect,
    const QskArcMetrics& metrics, bool radial, qreal borderWidth, QSGGeometry& geometry )
{
    geometry.setDrawingMode( QSGGeometry::DrawTriangleStrip );
    geometry.markVertexDataDirty();

    const Renderer renderer( rect, metrics, radial, QskRgb::Black, 0 );

    const auto lines = qskAllocateLines( geometry, renderer.fillCount() );
    if ( lines )
    {
        QskVertex::Line* border = nullptr;
        renderer.renderArc( metrics.thickness(), borderWidth, lines, border );
    }
}
