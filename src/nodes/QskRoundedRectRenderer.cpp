/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskRoundedRectRenderer.h"

#include "QskBoxBorderColors.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxRendererColorMap.h"
#include "QskBoxShapeMetrics.h"
#include "QskGradientDirection.h"
#include "QskRectRenderer.h"
#include "QskRoundedRect.h"

#include <qmath.h>
#include <qsggeometry.h>

using namespace QskVertex;

namespace
{
    class Vector1D
    {
      public:
        Vector1D() = default;

        inline constexpr Vector1D( qreal origin, qreal length )
            : origin( origin )
            , length( length )
        {
        }

        inline qreal valueAt( qreal t ) const
        {
            return origin + t * length;
        }

        qreal origin = 0.0;
        qreal length = 0.0;
    };

    /*
        A contour iterator for vertical and horizontal linear gradients.
        The radii in direction of the gradient need to match at the
        opening and at the closing sides,
     */
    class HVRectEllipseIterator
    {
      public:
        HVRectEllipseIterator(
                const QskRoundedRect::Metrics& metrics, const QLineF& vector )
            : m_vertical( vector.x1() == vector.x2() )
        {
            using namespace QskRoundedRect;

            const int* c;

            if ( m_vertical )
            {
                static const int cV[] = { TopLeft, TopRight, BottomLeft, BottomRight };
                c = cV;

                m_pos0 = metrics.innerQuad.top;
                m_size = metrics.innerQuad.height;

                m_t = m_pos0 + vector.y1() * m_size;
                m_dt = vector.dy() * m_size;
            }
            else
            {
                static const int cH[] = { TopLeft, BottomLeft, TopRight, BottomRight };
                c = cH;

                m_pos0 = metrics.innerQuad.left;
                m_size = metrics.innerQuad.width;

                m_t = m_pos0 + vector.x1() * m_size;
                m_dt = vector.dx() * m_size;
            }

            const auto& mc1 = metrics.corner[ c[0] ];
            const auto& mc2 = metrics.corner[ c[1] ];
            const auto& mc3 = ( mc1.stepCount >= mc2.stepCount ) ? mc1 : mc2;

            const auto& mc4 = metrics.corner[ c[2] ];
            const auto& mc5 = metrics.corner[ c[3] ];
            const auto& mc6 = ( mc4.stepCount >= mc5.stepCount ) ? mc4 : mc5;

            m_vector[0] = vectorAt( !m_vertical, false, mc1 );
            m_vector[1] = vectorAt( !m_vertical, true, mc2 );
            m_vector[2] = vectorAt( m_vertical, false, mc3 );

            m_vector[3] = vectorAt( !m_vertical, false, mc4 );
            m_vector[4] = vectorAt( !m_vertical, true, mc5 );
            m_vector[5] = vectorAt( m_vertical, true, mc6 );

            m_stepCounts[0] = mc3.stepCount;
            m_stepCounts[1] = mc6.stepCount;

            m_v1.from = m_vector[0].valueAt( 1.0 );
            m_v1.to = m_vector[1].valueAt( 1.0 );
            m_v1.pos = m_pos0;

            m_v2 = m_v1;

            m_arcIterator.reset( m_stepCounts[0], false );
        }

        inline bool advance()
        {
            auto v = m_vector;

            if ( m_arcIterator.step() == m_arcIterator.stepCount() )
            {
                if ( m_arcIterator.isInverted() )
                {
                    // we have finished the closing "corners"
                    return false;
                }

                m_arcIterator.reset( m_stepCounts[1], true );

                const qreal pos1 = v[2].valueAt( 0.0 );
                const qreal pos2 = v[5].valueAt( 0.0 );

                if ( pos1 < pos2 )
                {
                    // the real rectangle - between the rounded "corners "
                    m_v1 = m_v2;

                    m_v2.from = v[3].valueAt( 1.0 );
                    m_v2.to = v[4].valueAt( 1.0 );
                    m_v2.pos = pos2;

                    return true;
                }
            }

            m_arcIterator.increment();

            m_v1 = m_v2;

            if ( m_arcIterator.isInverted() )
                v += 3;

            m_v2.from = v[0].valueAt( m_arcIterator.cos() );
            m_v2.to = v[1].valueAt( m_arcIterator.cos() );
            m_v2.pos = v[2].valueAt( m_arcIterator.sin() );

            return true;
        }

        inline void setGradientLine( qreal value, Color color, ColoredLine* line )
        {
            const auto pos = m_t + value * m_dt;

            const qreal f = ( pos - m_v1.pos ) / ( m_v2.pos - m_v1.pos );

            const qreal v1 = m_v1.from + f * ( m_v2.from - m_v1.from );
            const qreal v2 = m_v1.to + f * ( m_v2.to - m_v1.to );

            setLine( v1, v2, pos, color, line );
        }

        inline void setContourLine( Color color, ColoredLine* line )
        {
            setLine( m_v2.from, m_v2.to, m_v2.pos, color, line );
        }

        inline qreal value() const
        {
            // coordinate translated into the gradient vector
            return ( m_v2.pos - m_t ) / m_dt;
        }

      private:

        inline Vector1D vectorAt( bool vertical, bool increasing,
            const QskRoundedRect::Metrics::Corner& c ) const
        {
            qreal center, radius;

            if ( vertical )
            {
                center = c.centerY;
                radius = c.radiusInnerY;
            }
            else
            {
                center = c.centerX;
                radius = c.radiusInnerX;
            }

            const qreal f = increasing ? 1.0 : -1.0;

            if ( radius < 0.0 )
            {
                center += radius * f;
                radius = 0.0;
            }
            else
            {
                radius *= f;
            }

            return { center, radius };
        }

        inline void setLine( qreal from, qreal to, qreal pos,
            Color color, ColoredLine* line )
        {
            if ( m_vertical )
                line->setLine( from, pos, to, pos, color );
            else
                line->setLine( pos, from, pos, to, color );
        }

        const bool m_vertical;

        int m_stepCounts[2];
        ArcIterator m_arcIterator;

        /*
            This iterator supports shapes, where we have the same radius in
            direction of the gradient ( exception: one corner is not rounded ).
            However we allow having different radii opposite to the direction
            of the gradient. So we have 3 center/radius pairs to calculate the
            interpolating contour lines at both ends ( opening/closing ).
         */
        Vector1D m_vector[6];

        /*
            position of the previous and following contour line, so that
            the positions of the gradiet lines in between can be calculated.
         */
        struct
        {
            qreal from, to; // opposite to the direction of the gradient
            qreal pos;      // in direction of the gradient
        } m_v1, m_v2;

        qreal m_pos0, m_size;
        qreal m_t, m_dt; // to translate into gradient values
    };
}


static inline Qt::Orientation qskQtOrientation( const QskGradient& gradient )
{
    return gradient.linearDirection().isVertical() ? Qt::Vertical : Qt::Horizontal;
}

static inline int qskFillLineCount(
    const QskRoundedRect::Metrics& metrics, const QskGradient& gradient )
{
    using namespace QskRoundedRect;

    if ( !gradient.isVisible() )
        return 0;

    int lineCount = 0;

    const auto dir = gradient.linearDirection();

    if ( dir.isVertical() )
    {
        lineCount += qMax( metrics.corner[ TopLeft ].stepCount,
            metrics.corner[ TopRight ].stepCount ) + 1;

        lineCount += qMax( metrics.corner[ BottomLeft ].stepCount,
            metrics.corner[ BottomRight ].stepCount ) + 1;

        if ( metrics.centerQuad.top >= metrics.centerQuad.bottom )
            lineCount--;
    }
    else if ( dir.isHorizontal() )
    {
        lineCount += qMax( metrics.corner[ TopLeft ].stepCount,
            metrics.corner[ BottomLeft ].stepCount ) + 1;

        lineCount += qMax( metrics.corner[ TopRight ].stepCount,
            metrics.corner[ BottomRight ].stepCount ) + 1;

        if ( metrics.centerQuad.left >= metrics.centerQuad.right )
            lineCount--;
    }
    else
    {
        lineCount += 2 * ( metrics.corner[ 0 ].stepCount + 1 );

        if ( metrics.centerQuad.left >= metrics.centerQuad.right )
            lineCount--;

        if ( metrics.centerQuad.top >= metrics.centerQuad.bottom )
            lineCount--;

        /*
            For diagonal lines the points at the opposite
            side are no points interpolating the outline.
            So we need to insert interpolating lines on both sides
         */

        lineCount *= 2; // a real ellipse could be done with lineCount lines: TODO ...

#if 1
        /*
            The termination of the fill algorithm is a bit random
            and might result in having an additional line.
            Until this effect is completely understood, we better
            reserve memory for this to avoid crashes.
         */

        lineCount++; // happens in a corner case - needs to be understood: TODO
#endif
    }

    // adding vertexes for the stops - beside the first/last

    if ( !gradient.isMonochrome() )
        lineCount += gradient.stepCount() - 1;

    return lineCount;
}

static inline void qskRenderBorder( const QskRoundedRect::Metrics& metrics,
    Qt::Orientation orientation, const QskBoxBorderColors& colors, ColoredLine* lines )
{
    QskRoundedRect::Stroker stroker( metrics );
    stroker.createBorderLines( orientation, lines, colors );
}

static inline void qskRenderUniformBox(
    const QskRoundedRect::Metrics& metrics, const QskBoxBorderColors& borderColors,
    const QskGradient& gradient, ColoredLine* fillLines, ColoredLine* borderLines )
{
    QskRoundedRect::Stroker stroker( metrics );
    stroker.createUniformBox( borderLines, borderColors, fillLines, gradient );
}

static inline void qskRenderFillOrdered(
    const QskRoundedRect::Metrics& metrics,
    const QskGradient& gradient, int lineCount, ColoredLine* lines )
{
    /*
        The algo for irregular radii at opposite corners is not yet
        implemented TODO ...
     */

    const auto dir = gradient.linearDirection();

    HVRectEllipseIterator it( metrics, dir.vector() );
    QskVertex::fillBox( it, gradient, lineCount,lines );
}

void QskRoundedRectRenderer::renderBorderGeometry(
    const QRectF& rect, const QskBoxShapeMetrics& shape,
    const QskBoxBorderMetrics& border, QSGGeometry& geometry )
{
    const QskRoundedRect::Metrics metrics( rect, shape, border );

    if ( metrics.innerQuad == metrics.outerQuad )
    {
        allocateLines< Line >( geometry, 0 );
        return;
    }

    const int stepCount = metrics.corner[ 0 ].stepCount;
    const int lineCount = 4 * ( stepCount + 1 ) + 1;

    const auto lines = allocateLines< Line >( geometry, lineCount );

    QskRoundedRect::Stroker stroker( metrics );
    stroker.createBorderLines( lines );
}

void QskRoundedRectRenderer::renderFillGeometry(
    const QRectF& rect, const QskBoxShapeMetrics& shape,
    const QskBoxBorderMetrics& border, QSGGeometry& geometry )
{
    using namespace QskRoundedRect;

    const Metrics metrics( rect, shape, border );

    if ( ( metrics.innerQuad.width <= 0 ) || ( metrics.innerQuad.height <= 0 ) )
    {
        geometry.allocate( 0 );
        return;
    }

    if ( metrics.isTotallyCropped )
    {
        // degenerated to a rectangle

        geometry.allocate( 4 );

        const auto& quad = metrics.innerQuad;

        auto p = geometry.vertexDataAsPoint2D();
        p[0].set( quad.left, quad.top );
        p[1].set( quad.right, quad.top );
        p[2].set( quad.left, quad.bottom );
        p[3].set( quad.right, quad.bottom );

        return;
    }

#if 0
    if (  metrics.isRadiusRegular )
    {
        int lineCount += metrics.corner[ TopLeft ].stepCount +
            metrics.corner[ BottomLeft ].stepCount;

        if ( metrics.centerQuad.top >= metrics.centerQuad.bottom )
            lineCount--;

        geometry.allocate( 2 * lineCount );

        Stroker< Line > stroker( metrics );
        ...

        return;
    }
#endif

    /*
        Unfortunately QSGGeometry::DrawTriangleFan is no longer supported with
        Qt6 and we have to go with DrawTriangleStrip, duplicating the center with
        each vertex.
     */

    const auto numPoints =
        metrics.corner[0].stepCount + metrics.corner[1].stepCount
        + metrics.corner[2].stepCount + metrics.corner[3].stepCount + 4;

    /*
        points: center point + interpolated corner points
        indexes: lines between the center and each point, where
                 the first line needs to be appended to close the filling
     */

    geometry.allocate( 1 + numPoints, 2 * ( numPoints + 1 ) );

    Q_ASSERT( geometry.sizeOfIndex() == 2 );

    auto p = geometry.vertexDataAsPoint2D();
    auto idx = geometry.indexDataAsUShort();

    int i = 0;

    p[i++].set( rect.x() + 0.5 * rect.width(), rect.y() + 0.5 * rect.height() );

    BorderValues v( metrics );

    {
        constexpr auto id = TopLeft;
        const auto& c = metrics.corner[ id ];

        for ( ArcIterator it( c.stepCount, false ); !it.isDone(); ++it )
        {
            *idx++ = 0;
            *idx++ = i;

            v.setAngle( it.cos(), it.sin() );
            p[i++].set( c.centerX - v.dx1( id ), c.centerY - v.dy1( id ) );
        }
    }
    {
        constexpr auto id = BottomLeft;
        const auto& c = metrics.corner[ id ];

        for ( ArcIterator it( c.stepCount, true ); !it.isDone(); ++it )
        {
            *idx++ = 0;
            *idx++ = i;

            v.setAngle( it.cos(), it.sin() );
            p[i++].set( c.centerX - v.dx1( id ), c.centerY + v.dy1( id ) );
        }
    }
    {
        constexpr auto id = BottomRight;
        const auto& c = metrics.corner[ id ];

        for ( ArcIterator it( c.stepCount, false ); !it.isDone(); ++it )
        {
            *idx++ = 0;
            *idx++ = i;

            v.setAngle( it.cos(), it.sin() );
            p[i++].set( c.centerX + v.dx1( id ), c.centerY + v.dy1( id ) );
        }
    }
    {
        constexpr auto id = TopRight;
        const auto& c = metrics.corner[ id ];

        for ( ArcIterator it( c.stepCount, true ); !it.isDone(); ++it )
        {
            *idx++ = 0;
            *idx++ = i;

            v.setAngle( it.cos(), it.sin() );
            p[i++].set( c.centerX + v.dx1( id ), c.centerY - v.dy1( id ) );
        }
    }

    *idx++ = 0;
    *idx++ = 1;
}

void QskRoundedRectRenderer::renderRect( const QRectF& rect,
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& border,
    const QskBoxBorderColors& borderColors, const QskGradient& gradient,
    QSGGeometry& geometry )
{
    using namespace QskRoundedRect;

    const Metrics metrics( rect, shape, border );

    const auto isTilted = gradient.linearDirection().isTilted();

    int fillLineCount = 0;

    if ( !metrics.innerQuad.isEmpty() && gradient.isVisible() )
    {
        if ( metrics.isTotallyCropped )
        {
            // degenerated to a rectangle

            fillLineCount = gradient.stepCount() + 1;

#if 1
            if ( isTilted )
            {
                if ( metrics.centerQuad.width == metrics.centerQuad.height )
                {
                    if ( !gradient.hasStopAt( 0.5 ) )
                        fillLineCount++;
                }
                else
                {
                    // we might need extra lines for the corners
                    fillLineCount += 2;
                }
            }
#endif
        }
        else
        {
            fillLineCount = qskFillLineCount( metrics, gradient );
        }
    }

    const int stepCount = metrics.corner[ 0 ].stepCount;

    int borderLineCount = 0;

    if ( borderColors.isVisible() && metrics.innerQuad != metrics.outerQuad )
    {
        borderLineCount = 4 * ( stepCount + 1 ) + 1;
        borderLineCount += extraBorderStops( borderColors );
    }

    int lineCount = borderLineCount + fillLineCount;

    bool extraLine = false;
    if ( borderLineCount > 0 && fillLineCount > 0 )
    {
        if ( !gradient.isMonochrome() && isTilted )
        {
            /*
                The filling ends at 45° and we have no implementation
                for creating the border from there. So we need to
                insert an extra dummy line to connect fill and border
             */
            extraLine = true;
            lineCount++;
        }
    }

    auto lines = allocateLines< ColoredLine >( geometry, lineCount );

    bool isUniform = true;
    if ( fillLineCount > 0 )
    {
        if ( metrics.isTotallyCropped )
        {
            isUniform = false;
        }
        else if ( !gradient.isMonochrome() )
        {
            if ( gradient.stepCount() > 1 || isTilted )
                isUniform = false;
        }

        if ( isUniform )
        {
            if ( !metrics.isRadiusRegular )
            {
                /*
                    When we have a combination of corners with the same
                    or no radius we could use the faster random algo: TODO ...
                 */
                isUniform = false;
            }
        }
    }

    if ( ( fillLineCount > 0 ) && ( borderLineCount > 0 ) )
    {
        if ( isUniform )
        {
            qskRenderUniformBox( metrics, borderColors,
                gradient, lines, lines + fillLineCount );
        }
        else
        {
            if ( metrics.isTotallyCropped )
            {
                QskRectRenderer::renderFill0( metrics.innerQuad,
                    gradient, fillLineCount, lines );
            }
            else if ( isTilted )
            {
                renderDiagonalFill( metrics, gradient, fillLineCount, lines );
            }
            else
            {
                qskRenderFillOrdered( metrics, gradient, fillLineCount, lines );
            }

            auto borderLines = lines + fillLineCount;
            if ( extraLine )
                borderLines++;

            const auto orientation = qskQtOrientation( gradient );
            qskRenderBorder( metrics, orientation, borderColors, borderLines );

            if ( extraLine )
            {
                const auto l = lines + fillLineCount;
                l[ 0 ].p1 = l[ -1 ].p2;
                l[ 0 ].p2 = l[ 1 ].p1;
            }
        }
    }
    else if ( fillLineCount > 0 )
    {
        if ( isUniform )
        {
            QskRoundedRect::Stroker stroker( metrics );
            stroker.createFillLines( lines, gradient );
        }
        else
        {
            if ( metrics.isTotallyCropped )
            {
                QskRectRenderer::renderFill0( metrics.innerQuad,
                    gradient, fillLineCount, lines );
            }
            else if ( isTilted )
            {
                renderDiagonalFill( metrics, gradient, fillLineCount, lines );
            }
            else
            {
                qskRenderFillOrdered( metrics, gradient, fillLineCount, lines );
            }
        }
    }
    else if ( borderLineCount > 0 )
    {
        qskRenderBorder( metrics, Qt::Vertical, borderColors, lines );
    }
}
