/**********************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskArcNode.h"
#include "QskArcRenderer.h"

QskArcNode::QskArcNode()
{
}

QskArcNode::~QskArcNode()
{
}

void QskArcNode::setArcData( const QRectF& rect, const QskArcMetrics& metrics,
    const QskArcBorderMetrics& borderMetrics, const QskGradient &gradient,
    QQuickWindow* window )
{
    m_metrics = metrics;
    m_borderMetrics = borderMetrics;
    m_gradient = gradient;

    if ( rect.isEmpty() )
    {
        return;
    }

    bool hasFill = gradient.isValid();
    bool hasBorder = !borderMetrics.isNull();

    if ( hasBorder )
    {
        /*
            Wrong as the border width should have an
            effect - even if not being visible. TODO ...
         */

        // ### implement border colors
//        hasBorder = borderColors.isVisible();
    }

    if ( !hasBorder && !hasFill )
    {
        return;
    }

    update( window, QskTextureRenderer::AutoDetect, rect.toRect() );
}

void QskArcNode::paint( QPainter* painter, const QSizeF &size )
{
    const qreal w = m_metrics.width();
    const QRectF rect( 0.5 * w, 0.5 * w, size.width() - w, size.height() - w );

    QskArcRenderer renderer;

    renderer.renderArc( rect, m_metrics, m_borderMetrics, m_gradient, painter );
}

uint QskArcNode::hash() const
{
    uint h = m_metrics.hash();
    h = m_borderMetrics.hash( h );

    for( const auto& stop : m_gradient.stops() )
        h = stop.hash( h );

    return h;
}
