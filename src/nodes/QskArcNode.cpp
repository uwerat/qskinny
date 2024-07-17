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
#include "QskStrokeNode.h"
#include "QskSGNode.h"
#include "QskShadowMetrics.h"

#include <qpainterpath.h>

#define ARC_BORDER_NODE

#ifdef ARC_BORDER_NODE
    using BorderNode = QskArcRenderNode;
#else
    #include <qpen.h>
    using BorderNode = QskStrokeNode;
#endif

namespace
{
    enum NodeRole
    {
        ShadowRole,

        PathRole,
        ArcRole,

        BorderRole
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

static void qskUpdateChildren( QSGNode* parentNode, quint8 role, QSGNode* node )
{
    static const QVector< quint8 > roles =
        { ShadowRole, PathRole, ArcRole, BorderRole };

    auto oldNode = QskSGNode::findChildNode( parentNode, role );
    QskSGNode::replaceChildNode( roles, role, parentNode, oldNode, node );
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

    auto shadowNode = static_cast< QskArcShadowNode* >(
        QskSGNode::findChildNode( this, ShadowRole ) );

    auto pathNode = static_cast< QskShapeNode* >(
        QskSGNode::findChildNode( this, PathRole ) );

    auto arcNode = static_cast< QskArcRenderNode* >(
        QskSGNode::findChildNode( this, ArcRole ) );

    auto borderNode = static_cast< BorderNode* >(
        QskSGNode::findChildNode( this, BorderRole ) );

    if ( metricsArc.isNull() || rect.isEmpty() )
    {
        delete shadowNode;
        delete pathNode;
        delete arcNode;
        delete borderNode;
        return;
    }

    const auto isFillNodeVisible = gradient.isVisible();
    const auto isBorderNodeVisible = ( borderWidth > 0.0 ) && ( borderColor.alpha() > 0 );
    const auto isShadowNodeVisible = isFillNodeVisible &&
        shadowColor.isValid() && ( shadowColor.alpha() > 0.0 );

    if ( isShadowNodeVisible )
    {
        if ( shadowNode == nullptr )
        {
            shadowNode = new QskArcShadowNode;
            QskSGNode::setNodeRole( shadowNode, ShadowRole );
        }

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
    else
    {
        delete shadowNode;
        shadowNode = nullptr;
    }

    if ( isFillNodeVisible )
    {
        if ( QskArcRenderer::isGradientSupported( gradient ) )
        {
            delete pathNode;
            pathNode = nullptr;

            if ( arcNode == nullptr )
            {
                arcNode = new QskArcRenderNode;
                QskSGNode::setNodeRole( arcNode, ArcRole );
            }

            arcNode->updateNode( rect, metricsArc, radial,
                borderWidth, QColor(), gradient );
        }
        else
        {
            delete arcNode;
            arcNode = nullptr;

            if ( pathNode == nullptr )
            {
                pathNode = new QskShapeNode;
                QskSGNode::setNodeRole( pathNode, PathRole );
            }

            const auto path = metricsArc.painterPath( rect, radial );
            pathNode->updateNode( path, QTransform(), rect,
                qskEffectiveGradient( gradient, metricsArc ) );
        }
    }
    else
    {
        delete pathNode;
        pathNode = nullptr;

        delete arcNode;
        arcNode = nullptr;
    }

    if ( isBorderNodeVisible )
    {
        if ( borderNode == nullptr )
        {
            borderNode = new BorderNode;
            QskSGNode::setNodeRole( borderNode, BorderRole );
        }

#ifdef ARC_BORDER_NODE
        borderNode->updateNode( rect, metricsArc, radial,
            borderWidth, borderColor, QskGradient() );
#else
        QPen pen( borderColor, borderWidth );
        pen.setCapStyle( Qt::FlatCap );

        const auto path = metricsArc.painterPath( rect, radial );
        borderNode->updateNode( path, QTransform(), pen );
#endif
    }
    else
    {
        delete borderNode;
        borderNode = nullptr;
    }

    qskUpdateChildren( this, ShadowRole, shadowNode );
    qskUpdateChildren( this, PathRole, pathNode );
    qskUpdateChildren( this, ArcRole, arcNode );
    qskUpdateChildren( this, BorderRole, borderNode );
}
