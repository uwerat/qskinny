/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxNode.h"
#include "QskBoxFillNode.h"
#include "QskBoxShadowNode.h"
#include "QskBoxRectangleNode.h"
#include "QskBoxRenderer.h"
#include "QskSGNode.h"

#include "QskGradient.h"
#include "QskGradientDirection.h"
#include "QskShadowMetrics.h"
#include "QskBoxBorderMetrics.h"
#include "QskBoxBorderColors.h"

namespace
{
    enum Role
    {
        ShadowRole,
        BoxRole,
        FillRole
    };
}

void qskUpdateChildren( QSGNode* parentNode, quint8 role, QSGNode* node )
{
    static const QVector< quint8 > roles = { ShadowRole, BoxRole, FillRole };

    auto oldNode = QskSGNode::findChildNode( parentNode, role );
    QskSGNode::replaceChildNode( roles, role, parentNode, oldNode, node );
}

template< typename Node >
inline Node* qskNode( QSGNode* parentNode, quint8 role )
{
    using namespace QskSGNode;

    auto node = static_cast< Node* > ( findChildNode( parentNode, role ) );

    if ( node == nullptr )
    {
        node = new Node();
        setNodeRole( node, role );
    }

    return node;
}

QskBoxNode::QskBoxNode()
{
}

QskBoxNode::~QskBoxNode()
{
}

void QskBoxNode::updateNode( const QRectF& rect,
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& borderMetrics,
    const QskBoxBorderColors& borderColors, const QskGradient& gradient,
    const QskShadowMetrics& shadowMetrics, const QColor& shadowColor )
{
    using namespace QskSGNode;

    QskBoxShadowNode* shadowNode = nullptr;
    QskBoxRectangleNode* rectNode = nullptr;
    QskBoxFillNode* fillNode = nullptr;

    if ( !shadowMetrics.isNull()
        && shadowColor.isValid() && shadowColor.alpha() != 0 )
    {
        shadowNode = qskNode< QskBoxShadowNode >( this, ShadowRole );
        shadowNode->setShadowData( shadowMetrics.shadowRect( rect ),
            shape, shadowMetrics.blurRadius(), shadowColor );
    }

    /*
        QskBoxRectangleNode supports vertical/horizontal/diagonal gradients only.
        If our gradient doesn't fall into this category we use a QskBoxFillNode.
        However the border is always done with a QskBoxRectangleNode
     */

    if ( QskBoxRenderer::isGradientSupported( shape, gradient ) )
    {
        rectNode = qskNode< QskBoxRectangleNode >( this, BoxRole );
        rectNode->updateNode( rect, shape, borderMetrics, borderColors, gradient );
    }
    else
    {
        if ( !borderMetrics.isNull() && borderColors.isVisible() )
        {
            rectNode = qskNode< QskBoxRectangleNode >( this, BoxRole );
            rectNode->updateNode( rect, shape, borderMetrics, borderColors, QskGradient() );
        }

        if ( gradient.isVisible() )
        {
            fillNode = qskNode< QskBoxFillNode >( this, FillRole );
            fillNode->updateNode( rect, shape, borderMetrics, gradient );
        }
    }

    qskUpdateChildren( this, ShadowRole, shadowNode );
    qskUpdateChildren( this, BoxRole, rectNode );
    qskUpdateChildren( this, FillRole, fillNode );
}
