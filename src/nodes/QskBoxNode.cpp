/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskBoxNode.h"
#include "QskBoxShadowNode.h"
#include "QskBoxRectangleNode.h"
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

static void qskUpdateChildren( QSGNode* parentNode, quint8 role, QSGNode* node )
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
    QskBoxRectangleNode* fillNode = nullptr;

    if ( !rect.isEmpty() )
    {
        if ( !shadowMetrics.isNull()
            && shadowColor.isValid() && shadowColor.alpha() != 0 )
        {
            shadowNode = qskNode< QskBoxShadowNode >( this, ShadowRole );
            shadowNode->setShadowData( shadowMetrics.shadowRect( rect ),
                shape, shadowMetrics.blurRadius(), shadowColor );
        }

        if ( QskBoxRectangleNode::isCombinedGeometrySupported( gradient ) )
        {
            rectNode = qskNode< QskBoxRectangleNode >( this, BoxRole );
            rectNode->updateBox( rect, shape, borderMetrics, borderColors, gradient );
        }
        else
        {
            if ( !borderMetrics.isNull() && borderColors.isVisible() )
            {
                rectNode = qskNode< QskBoxRectangleNode >( this, BoxRole );
                rectNode->updateBorder( rect, shape, borderMetrics, borderColors );
            }

            if ( gradient.isVisible() )
            {
                fillNode = qskNode< QskBoxRectangleNode >( this, FillRole );
                fillNode->updateFilling( rect, shape, borderMetrics, gradient );
            }
        }
    }

    qskUpdateChildren( this, ShadowRole, shadowNode );
    qskUpdateChildren( this, BoxRole, rectNode );
    qskUpdateChildren( this, FillRole, fillNode );
}
