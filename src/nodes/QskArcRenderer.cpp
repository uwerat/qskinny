/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskArcRenderer.h"
#include "QskArcMetrics.h"
#include "QskGradient.h"
#include "QskVertex.h"

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
    const int length = qskApproximatedCircumference( rect );
    return std::max( 3, qCeil( length / 3.0 ) );
}

namespace
{
    class Stroker
    {
      public:
        Stroker( const QRectF&, const QskArcMetrics& metrics, qreal borderWidth );

        int fillCount() const;
        int borderCount() const;

        void setBorderLines( QskVertex::Line* ) const;

      private:
        const QRectF& m_rect;
        const QskArcMetrics& m_metrics;
        const qreal m_borderWidth;
    };
}

Stroker::Stroker( const QRectF& rect, const QskArcMetrics& metrics, qreal borderWidth )
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

int Stroker::borderCount() const
{
    if ( m_metrics.isNull() )
        return 0;

    int n = qskStepCount( m_rect );
    if ( !m_metrics.isClosed() )
    {
        n = qCeil( n * qAbs( m_metrics.spanAngle() ) / 360.0 );
        n += 1; // closing line
    }

    n *= 2; // inner/outer border
    n += 1; // dummy line connection inner/outer border

    return n;
}

void Stroker::setBorderLines( QskVertex::Line* lines ) const
{
    Q_UNUSED( lines );
}

void QskArcRenderer::renderBorderGeometry( const QRectF& rect,
    const QskArcMetrics& metrics, qreal borderWidth, QSGGeometry& geometry )
{
    geometry.setDrawingMode( QSGGeometry::DrawTriangleStrip );

    Stroker stroker( rect, metrics, borderWidth );

    const auto lines = qskAllocateLines( geometry, stroker.borderCount() );
    if ( lines )
        stroker.setBorderLines( lines );
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
