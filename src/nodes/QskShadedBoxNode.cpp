/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskShadedBoxNode.h"
#include "QskBoxShadowNode.h"
#include "QskShadowMetrics.h"
#include <qcolor.h>

QskShadedBoxNode::QskShadedBoxNode()
{
    m_boxNode.setFlag( QSGNode::OwnedByParent, false );
    appendChildNode( &m_boxNode );
}

QskShadedBoxNode::~QskShadedBoxNode()
{
}

void QskShadedBoxNode::setBoxData( const QRectF& rect,
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& borderMetrics,
    const QskBoxBorderColors& borderColors, const QskGradient& gradient,
    const QskShadowMetrics& shadowMetrics, const QColor& shadowColor )
{
    m_boxNode.setBoxData( rect, shape, borderMetrics, borderColors, gradient );
    setShadowData( rect, shape, shadowMetrics, shadowColor );
}

void QskShadedBoxNode::setShadowData(
    const QRectF& rect, const QskBoxShapeMetrics& shape,
    const QskShadowMetrics& metrics, const QColor& color )
{
    if ( metrics.isNull() || !color.isValid() || color.alpha() == 0 )
    {
        if ( m_shadowNode )
        {
            removeChildNode( m_shadowNode );
            delete m_shadowNode;
            m_shadowNode = nullptr;
        }
    }
    else
    {
        if ( m_shadowNode == nullptr )
        {
            m_shadowNode = new QskBoxShadowNode();
            insertChildNodeBefore( m_shadowNode, &m_boxNode );
        }

        m_shadowNode->setRect( metrics.shadowRect( rect ) );
        m_shadowNode->setShape( shape );
        m_shadowNode->setBlurRadius( metrics.blurRadius() );
        m_shadowNode->setClipShape( shape );

        m_shadowNode->updateGeometry();
    }
}
