/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskArcNode.h"
#include "QskArcMetrics.h"
#include "QskArcRenderer.h"
#include "QskGradient.h"

namespace
{
    class ArcData
    {
      public:
        const QskArcMetrics& metrics;
        const QskGradient& gradient;
    };
}

QskArcNode::QskArcNode()
{
}

QskArcNode::~QskArcNode()
{
}

void QskArcNode::setArcData( const QRectF& rect, const QskArcMetrics& metrics,
    const QskGradient& gradient, QQuickWindow* window )
{
    const ArcData arcData { metrics, gradient };
    update( window, rect, QSizeF(), &arcData );
}

void QskArcNode::paint( QPainter* painter, const QSize& size, const void* nodeData )
{
    const auto arcData = reinterpret_cast< const ArcData* >( nodeData );

    const qreal t = arcData->metrics.thickness();
    const QRectF rect( 0.5 * t, 0.5 * t, size.width() - t, size.height() - t );

    QskArcRenderer::renderArc( rect, arcData->metrics, arcData->gradient, painter );
}

QskHashValue QskArcNode::hash( const void* nodeData ) const
{
    const auto arcData = reinterpret_cast< const ArcData* >( nodeData );

    auto h = arcData->metrics.hash();
    return arcData->gradient.hash( h );
}
