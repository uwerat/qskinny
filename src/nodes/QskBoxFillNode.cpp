/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskBoxFillNode.h"
#include "QskGradientMaterial.h"
#include "QskGradient.h"
#include "QskGradientDirection.h"
#include "QskBoxShapeMetrics.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxRenderer.h"
#include "QskFillNodePrivate.h"

static inline QskHashValue qskMetricsHash(
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& borderMetrics )
{
    QskHashValue hash = 13000;

    hash = shape.hash( hash );
    return borderMetrics.hash( hash );
}

class QskBoxFillNodePrivate final : public QskFillNodePrivate
{
  public:
    inline void resetValues()
    {
        rect = QRectF();
        gradientHash = 0;
        metricsHash = 0;
    }

    QRectF rect;

    QskHashValue gradientHash = 0;
    QskHashValue metricsHash = 0;
};

QskBoxFillNode::QskBoxFillNode()
    : QskFillNode( *new QskBoxFillNodePrivate )
{
}

void QskBoxFillNode::updateNode( const QRectF& rect, const QskGradient& gradient )
{
    updateNode( rect, QskBoxShapeMetrics(), QskBoxBorderMetrics(), gradient );
}

void QskBoxFillNode::updateNode(
    const QRectF& rect, const QskBoxShapeMetrics& shapeMetrics,
    const QskBoxBorderMetrics& borderMetrics, const QskGradient& gradient )
{
    Q_D( QskBoxFillNode );

    if ( rect.isEmpty() || !gradient.isVisible() )
    {
        d->resetValues();
        resetGeometry();

        return;
    }

    const auto metricsHash = qskMetricsHash( shapeMetrics, borderMetrics );
    const auto gradientHash = gradient.hash( 22879 );

    const bool dirtyColors = gradientHash != d->gradientHash;
    const bool dirtyMetrics = ( metricsHash != d->metricsHash ) || ( rect != d->rect );

    d->metricsHash = metricsHash;
    d->gradientHash = gradientHash;
    d->rect = rect;

    if ( gradient.isMonochrome() )
    {
        if ( dirtyColors )
            setColoring( gradient.startColor().toRgb() );
    }
    else
    {
        // dirtyMetrics: the shader also depends on the target rectangle !

        if ( dirtyColors || dirtyMetrics )
            setColoring( rect, gradient.effectiveGradient() );
    }

    if ( dirtyMetrics )
    {
        QskBoxRenderer::renderFillGeometry(
            rect, shapeMetrics, borderMetrics, *geometry() );

        markDirty( QSGNode::DirtyGeometry );
        geometry()->markVertexDataDirty();
    }
}
