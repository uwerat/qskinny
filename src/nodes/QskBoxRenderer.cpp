/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskBoxRenderer.h"
#include "QskBoxShapeMetrics.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxBorderColors.h"
#include "QskBoxMetrics.h"
#include "QskBoxBasicStroker.h"
#include "QskBoxGradientStroker.h"

#include "QskGradient.h"
#include "QskGradientDirection.h"
#include "QskFunctions.h"

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

static inline QskGradient qskEffectiveGradient(
    const QRectF& rect, const QskGradient& gradient )
{
    if ( !gradient.isVisible() )
        return gradient;

    const auto dir = gradient.linearDirection();

    auto g = gradient;

    if ( !dir.isTilted() )
    {
        /*
            Dealing with inverted gradient vectors makes the code even
            more unreadable. So we simply invert stops/vector instead.
         */
        if ( ( dir.x1() > dir.x2() ) || ( dir.y1() > dir.y2() ) )
        {
            g.setLinearDirection( dir.x2(), dir.y2(), dir.x1(), dir.y1() );

            if ( !g.isMonochrome() )
                g.setStops( qskRevertedGradientStops( gradient.stops() ) );
        }
    }

    if ( g.stretchMode() == QskGradient::StretchToSize )
        g.stretchTo( rect );

    return g;
}

static inline bool qskMaybeSpreading( const QskGradient& gradient )
{
    if ( gradient.stretchMode() == QskGradient::StretchToSize )
        return !gradient.linearDirection().contains( QRectF( 0, 0, 1, 1 ) );

    return true;
}

QskBoxRenderer::QskBoxRenderer( const QQuickWindow* window )
    : m_window( window )
{
}

QskBoxRenderer::~QskBoxRenderer()
{
}

bool QskBoxRenderer::isGradientSupported( const QskGradient& gradient )
{
    if ( !gradient.isVisible() || gradient.isMonochrome() )
        return true;

    switch( gradient.type() )
    {
        case QskGradient::Stops:
        {
            // will be rendered as vertical linear gradient
            return true;
        }
        case QskGradient::Linear:
        {
            if ( ( gradient.spreadMode() != QskGradient::PadSpread )
                && qskMaybeSpreading( gradient ) )
            {
                // shouldn't be hard to implement the other spreadModes TODO ...
                return false;
            }

            return true;
        }

        default:
        {
            /*
                At least Conical gradients could be implemented easily TODO..

                For the moment Radial/Conical gradients have to be done
                with QskGradientMaterial
             */
            return false;
        }
    }

    return false;
}

void QskBoxRenderer::setBorderLines(
    const QRectF& rect, const QskBoxShapeMetrics& shape,
    const QskBoxBorderMetrics& border, QSGGeometry& geometry )
{
    geometry.setDrawingMode( QSGGeometry::DrawTriangleStrip );
    geometry.markVertexDataDirty();

    const QskBoxMetrics metrics( rect, shape, border );
    const QskBoxBasicStroker stroker( metrics );

    const auto lines = qskAllocateLines( geometry, stroker.borderCount() );
    if ( lines )
        stroker.setBorderLines( lines );
}

void QskBoxRenderer::setFillLines(
    const QRectF& rect, const QskBoxShapeMetrics& shape, QSGGeometry& geometry )
{
    setFillLines( rect, shape, QskBoxBorderMetrics(), geometry );
}

void QskBoxRenderer::setFillLines(
    const QRectF& rect, const QskBoxShapeMetrics& shape,
    const QskBoxBorderMetrics& border, QSGGeometry& geometry )
{
    geometry.setDrawingMode( QSGGeometry::DrawTriangleStrip );
    geometry.markVertexDataDirty();

    const QskBoxMetrics metrics( rect, shape, border );
    QskBoxBasicStroker stroker( metrics );

    if ( auto lines = qskAllocateLines( geometry, stroker.fillCount() ) )
        stroker.setFillLines( lines );
}

void QskBoxRenderer::setColoredFillLines( const QRectF& rect,
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& border,
    const QskGradient& gradient, QSGGeometry& geometry )
{
    setColoredBorderAndFillLines( rect, shape, border,
        QskBoxBorderColors(), gradient, geometry );
}

void QskBoxRenderer::setColoredBorderLines( const QRectF& rect,
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& border,
    const QskBoxBorderColors& borderColors, QSGGeometry& geometry )
{
    geometry.setDrawingMode( QSGGeometry::DrawTriangleStrip );
    geometry.markVertexDataDirty();

    const QskBoxMetrics metrics( rect, shape, border );
    const QskBoxBasicStroker stroker( metrics, borderColors );

    if ( auto lines = qskAllocateColoredLines( geometry, stroker.borderCount() ) )
        stroker.setBoxLines( lines, nullptr );
}

void QskBoxRenderer::setColoredBorderAndFillLines( const QRectF& rect,
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& border,
    const QskBoxBorderColors& borderColors, const QskGradient& gradient,
    QSGGeometry& geometry )
{
    geometry.setDrawingMode( QSGGeometry::DrawTriangleStrip );
    geometry.markVertexDataDirty();

    const QskBoxMetrics metrics( rect, shape, border );
    const auto effectiveGradient = qskEffectiveGradient( metrics.innerRect, gradient );

    if ( metrics.innerRect.isEmpty() ||
        QskVertex::ColorMap::isGradientSupported( effectiveGradient, metrics.innerRect ) )
    {
        /*
            The gradient can be translated to a QskBoxRenderer::ColorMap and we can do all
            coloring by adding a color info to points of the contour lines.
            The orientation of contour lines does not depend on the direction
            of the gradient vector.

            This allows using simpler and faster algos.
         */

        const QskBoxBasicStroker stroker( metrics, borderColors, effectiveGradient );

        const int fillCount = stroker.fillCount();
        const int borderCount = stroker.borderCount();

        if ( auto lines = qskAllocateColoredLines( geometry, borderCount + fillCount ) )
        {
            auto fillLines = fillCount ? lines : nullptr;
            auto borderLines = borderCount ? lines + fillCount : nullptr;

            stroker.setBoxLines( borderLines, fillLines );
        }
    }
    else
    {
        /*
            We need to create gradient and contour lines in the correct order
            perpendicular to the gradient vector.
         */
        const QskBoxBasicStroker borderStroker( metrics, borderColors );
        QskBoxGradientStroker fillStroker( metrics, effectiveGradient );

        const int fillCount = fillStroker.lineCount();
        const int borderCount = borderStroker.borderCount();
        const int extraLine = ( fillCount && borderCount ) ? 1 : 0;

        auto lines = qskAllocateColoredLines(
            geometry, fillCount + borderCount + extraLine );

        if ( fillCount )
            fillStroker.setLines( fillCount, lines );

        if ( borderCount )
            borderStroker.setBorderLines( lines + fillCount + extraLine );

        if ( extraLine )
        {
            // dummy line to connect filling and border
            const auto l = lines + fillCount;
            l[0].p1 = l[-1].p2;
            l[0].p2 = l[+1].p1;
        }
    }
}

QskGradient QskBoxRenderer::effectiveGradient( const QskGradient& gradient )
{
    if ( ( gradient.type() == QskGradient::Stops ) || gradient.isMonochrome() )
    {
        // the shader for linear gradients is the fastest

        auto g = gradient;
        g.setDirection( QskGradient::Linear );

        return g;
    }

    return gradient;
}
