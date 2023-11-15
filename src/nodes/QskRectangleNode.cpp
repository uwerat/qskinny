/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskRectangleNode.h"
#include "QskGradient.h"
#include "QskBoxRenderer.h"
#include "QskBoxShapeMetrics.h"
#include "QskFillNodePrivate.h"

class QskRectangleNodePrivate final : public QskFillNodePrivate
{
  public:
    inline void resetValues()
    {
        rect = QRectF();
        shape = QskBoxShapeMetrics();
        gradientHash = 0;
        metricsHash = 0;
    }

    QRectF rect;
    QskBoxShapeMetrics shape;

    QskHashValue gradientHash = 0;
    QskHashValue metricsHash = 0;
};

QskRectangleNode::QskRectangleNode()
    : QskFillNode( *new QskRectangleNodePrivate )
{
}

QskRectangleNode::~QskRectangleNode()
{
}

void QskRectangleNode::updateNode(
    const QRectF& rect, const QskGradient& gradient )
{
    updateNode( rect, QskBoxShapeMetrics(), gradient );
}

void QskRectangleNode::updateNode(
    const QRectF& rect, const QskBoxShapeMetrics& shape, const QskGradient& gradient )
{
    Q_D( QskRectangleNode );

    if ( rect.isEmpty() || !gradient.isVisible() )
    {
        d->resetValues();
        resetGeometry();

        return;
    }

    const auto effectiveGradient = gradient.effectiveGradient();
    const auto effectiveShape = shape.toAbsolute( rect.size() );

    const auto gradientHash = effectiveGradient.hash( 54228 );
    const auto metricsHash = effectiveShape.hash( 44564 );

    const bool dirtyColors = gradientHash != d->gradientHash;
    const bool dirtyMetrics = ( rect != d->rect ) || ( metricsHash != d->metricsHash );

    if ( !( dirtyColors || dirtyMetrics ) )
        return;

    d->gradientHash = gradientHash;
    d->metricsHash = metricsHash;
    d->rect = rect;
    d->shape = effectiveShape;

    if ( QskBox::isGradientSupported( effectiveShape, effectiveGradient ) )
    {
        setColoring( Polychrome );

        /*
            Colors are added to the vertices, while the material does
            not depend on the gradient at all
         */
        if ( dirtyMetrics || dirtyColors )
        {
            QskBox::renderBox( rect,
                effectiveShape, effectiveGradient, *geometry() );

            geometry()->markVertexDataDirty();
            markDirty( QSGNode::DirtyGeometry );
        }
    }
    else
    {
        if ( dirtyColors || dirtyMetrics )
            setColoring( rect, effectiveGradient );

        if ( dirtyMetrics )
        {
            QskBox::renderFillGeometry( rect, effectiveShape, *geometry() );

            geometry()->markVertexDataDirty();
            markDirty( QSGNode::DirtyGeometry );
        }
    }
}
