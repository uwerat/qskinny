/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskArcNode.h"
#include "QskArcMetrics.h"
#include "QskArcRenderNode.h"
#include "QskArcRenderer.h"
#include "QskMargins.h"
#include "QskGradient.h"
#include "QskSGNode.h"
#include "QskRgbValue.h"

namespace
{
    enum NodeRole
    {
        /*
            If possible border + filling will be displayed by ArcRole
            Otherwise ArcRole displays the border and FillRole the filling
         */

        ArcRole,
        FillRole
    };
}

static void qskUpdateChildren( QSGNode* parentNode, quint8 role, QSGNode* node )
{
    static const QVector< quint8 > roles = { ArcRole, FillRole };

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

QskArcNode::QskArcNode()
{
}

QskArcNode::~QskArcNode()
{
}

void QskArcNode::setArcData( const QRectF& rect,
    const QskArcMetrics& arcMetrics, const QskGradient& gradient )
{
    setArcData( rect, arcMetrics, 0.0, QColor(), gradient );
}

void QskArcNode::setArcData( const QRectF& rect, const QskArcMetrics& arcMetrics,
    const qreal borderWidth, const QColor& borderColor, const QskGradient& gradient )
{
    using namespace QskSGNode;

    QskArcRenderNode* arcNode = nullptr;
    QskArcRenderNode* fillNode = nullptr;

    if ( !( rect.isEmpty() || arcMetrics.isNull() ) )
    {
        const bool radial = false;
        const auto metricsArc = arcMetrics.toAbsolute( rect.size() );

        const auto hasFilling = gradient.isVisible();
        const auto hasBorder = ( borderWidth > 0.0 ) && QskRgb::isVisible( borderColor );

        if ( hasBorder || hasFilling )
        {
            arcNode = qskNode< QskArcRenderNode >( this, ArcRole );

            if ( hasBorder && hasFilling )
            {
                const bool doCombine = arcNode->hasHint( QskFillNode::PreferColoredGeometry )
                    && QskArcRenderer::isGradientSupported( rect, metricsArc, gradient );

                if ( !doCombine )
                    fillNode = qskNode< QskArcRenderNode >( this, FillRole );
            }

            if ( fillNode )
            {
                arcNode->updateBorder( rect, metricsArc,
                    radial, borderWidth, borderColor );

                fillNode->updateFilling( rect, metricsArc, radial, borderWidth, gradient );
            }
            else
            {
                arcNode->updateArc( rect, metricsArc,
                    radial, borderWidth, borderColor, gradient );
            }
        }
    }

    qskUpdateChildren( this, ArcRole, arcNode );
    qskUpdateChildren( this, FillRole, fillNode );
}
