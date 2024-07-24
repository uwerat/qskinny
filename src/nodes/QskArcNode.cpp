/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskArcNode.h"
#include "QskArcMetrics.h"
#include "QskArcShadowNode.h"
#include "QskArcRenderNode.h"
#include "QskArcRenderer.h"
#include "QskMargins.h"
#include "QskGradient.h"
#include "QskShapeNode.h"
#include "QskSGNode.h"
#include "QskShadowMetrics.h"

#include <qpainterpath.h>

namespace
{
    enum NodeRole
    {
        ShadowRole,

        PathRole,
        ArcRole
    };
}

static inline QskGradient qskEffectiveGradient(
    const QskGradient& gradient, const QskArcMetrics& metrics )
{
    if ( !gradient.isMonochrome() )
    {
        if ( gradient.type() == QskGradient::Stops )
        {
            QskGradient g( gradient.stops() );
            g.setConicDirection( 0.5, 0.5, metrics.startAngle(), 360.0 );

            return g;
        }
    }

    return gradient;
}

template< typename Node >
inline Node* qskInsertOrRemoveNode( QSGNode* parentNode, quint8 role, bool isValid )
{
    using namespace QskSGNode;

    Node* oldNode = static_cast< Node* >( findChildNode( parentNode, role ) );
    Node* newNode = isValid ? ensureNode< Node >( oldNode ) : nullptr;

    static const QVector< quint8 > roles = { ShadowRole, PathRole, ArcRole };
    replaceChildNode( roles, role, parentNode, oldNode, newNode );

    return newNode;
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
    setArcData( rect, arcMetrics, 0.0, QColor(), gradient, {}, {} );
}

void QskArcNode::setArcData( const QRectF& rect, const QskArcMetrics& arcMetrics,
    const qreal borderWidth, const QColor& borderColor, const QskGradient& gradient )
{
    setArcData( rect, arcMetrics, borderWidth, borderColor, gradient, {}, {} );
}

void QskArcNode::setArcData( const QRectF& rect, const QskArcMetrics& arcMetrics,
    const qreal borderWidth, const QColor& borderColor, const QskGradient& gradient,
    const QColor& shadowColor, const QskShadowMetrics& shadowMetrics )
{
    const bool radial = false;

    const auto metricsArc = arcMetrics.toAbsolute( rect.size() );

    if ( metricsArc.isNull() || rect.isEmpty() )
    {
        delete QskSGNode::findChildNode( this, ShadowRole );
        delete QskSGNode::findChildNode( this, PathRole );
        delete QskSGNode::findChildNode( this, ArcRole );

        return;
    }

    const auto hasFilling = gradient.isVisible();
    const auto hasBorder = ( borderWidth > 0.0 )
        && borderColor.isValid() && ( borderColor.alpha() > 0 );
    const auto hasShadow = shadowColor.isValid() && ( shadowColor.alpha() > 0 );

    auto shadowNode = qskInsertOrRemoveNode< QskArcShadowNode >(
        this, ShadowRole, hasFilling && hasShadow );

    if ( shadowNode )
    {
        /*
            The shader of the shadow node is for circular arcs and we have some
            unwanted scaling issues for the spread/blur values when having ellipsoid
            arcs. We might also want to add the spread value to the ends of the arc
            and not only to its radius. TODO ...
         */

        const auto sm = shadowMetrics.toAbsolute( rect.size() );
        const auto shadowRect = sm.shadowRect( rect );
        const auto spreadRadius = sm.spreadRadius() + 0.5 * metricsArc.thickness();

        shadowNode->setShadowData( shadowRect, spreadRadius, sm.blurRadius(),
            metricsArc.startAngle(), metricsArc.spanAngle(), shadowColor );
    }

    auto pathNode = qskInsertOrRemoveNode< QskShapeNode >(
        this, PathRole, hasFilling && !QskArcRenderer::isGradientSupported( gradient ) );

    if ( pathNode )
    {
        const auto path = metricsArc.painterPath( rect, radial );
        pathNode->updateNode( path, QTransform(), rect,
            qskEffectiveGradient( gradient, metricsArc ) );
    }

    auto arcNode = qskInsertOrRemoveNode< QskArcRenderNode >(
        this, ArcRole, hasBorder || ( hasFilling && !pathNode ) );

    if ( arcNode )
    {
        arcNode->updateNode( rect, metricsArc, radial,
            borderWidth, borderColor, pathNode ? QskGradient() : gradient );
    }
}
