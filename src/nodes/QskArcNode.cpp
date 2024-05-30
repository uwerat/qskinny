/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskArcNode.h"
#include "QskArcMetrics.h"
#include "QskArcShadowNode.h"
#include "QskArcRenderNode.h"
#include "QskMargins.h"
#include "QskGradient.h"
#include "QskShapeNode.h"
#include "QskStrokeNode.h"
#include "QskSGNode.h"
#include "QskShadowMetrics.h"

#include <qpainterpath.h>

// #define ARC_BORDER_NODE
#define ARC_FILL_NODE

#ifdef ARC_BORDER_NODE
    using BorderNode = QskArcRenderNode;
#else
    #include <qpen.h>
    using BorderNode = QskStrokeNode;
#endif

#ifdef ARC_FILL_NODE
    using FillNode = QskArcRenderNode;
#else
    using FillNode = QskShapeNode;
#endif

namespace
{
    enum NodeRole
    {
        ShadowRole,
        FillRole,
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

static inline QskArcMetrics qskEffectiveMetrics(
    const QskArcMetrics& metrics, const QRectF& rect )
{
    if ( metrics.sizeMode() == Qt::RelativeSize )
    {
        const auto rx = 0.5 * rect.width();
        const auto ry = 0.5 * rect.height();

        return metrics.toAbsolute( rx, ry );
    }

    return metrics;
}

static inline QRectF qskEffectiveRect(
    const QRectF& rect, const qreal borderWidth )
{
    if ( borderWidth <= 0.0 )
        return rect;

    return qskValidOrEmptyInnerRect( rect, QskMargins( 0.5 * borderWidth ) );
}

static void qskUpdateChildren( QSGNode* parentNode, quint8 role, QSGNode* node )
{
    static const QVector< quint8 > roles = { ShadowRole, FillRole, BorderRole };

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
    const QskArcMetrics& arcMetrics, const QskGradient& fillGradient )
{
    setArcData( rect, arcMetrics, 0.0, QColor(), fillGradient, {}, {} );
}

void QskArcNode::setArcData( const QRectF& rect, const QskArcMetrics& arcMetrics,
    const qreal borderWidth, const QColor& borderColor, const QskGradient& fillGradient )
{
    setArcData( rect, arcMetrics, borderWidth, borderColor, fillGradient, {}, {} );
}

void QskArcNode::setArcData( const QRectF& rect, const QskArcMetrics& arcMetrics,
    const qreal borderWidth, const QColor& borderColor, const QskGradient& fillGradient,
    const QColor& shadowColor, const QskShadowMetrics& shadowMetrics )
{
    const auto metricsArc = qskEffectiveMetrics( arcMetrics, rect );
    const auto gradient = qskEffectiveGradient( fillGradient, metricsArc );

    auto shadowNode = static_cast< QskArcShadowNode* >(
        QskSGNode::findChildNode( this, ShadowRole ) );

    auto fillNode = static_cast< FillNode* >(
        QskSGNode::findChildNode( this, FillRole ) );

    auto borderNode = static_cast< BorderNode* >(
        QskSGNode::findChildNode( this, BorderRole ) );

    const auto arcRect = qskEffectiveRect( rect, borderWidth );
    if ( arcRect.isEmpty() )
    {
        delete shadowNode;
        delete fillNode;
        delete borderNode;
        return;
    }

    const auto isFillNodeVisible = gradient.isVisible() && !metricsArc.isNull();
    const auto isStrokeNodeVisible = ( borderWidth > 0.0 ) && ( borderColor.alpha() > 0 );
    const auto isShadowNodeVisible = isFillNodeVisible &&
        shadowColor.isValid() && ( shadowColor.alpha() > 0.0 );

    const auto path = metricsArc.painterPath( arcRect );

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

        const auto sm = shadowMetrics.toAbsolute( arcRect.size() );
        const auto shadowRect = sm.shadowRect( arcRect );
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
        if ( fillNode == nullptr )
        {
            fillNode = new FillNode;
            QskSGNode::setNodeRole( fillNode, FillRole );
        }

#ifdef ARC_FILL_NODE
        fillNode->updateNode( arcRect, metricsArc, gradient );
#else
        fillNode->updateNode( path, QTransform(), arcRect, gradient );
#endif
    }
    else
    {
        delete fillNode;
        fillNode = nullptr;
    }

    if ( isStrokeNodeVisible )
    {
        if ( borderNode == nullptr )
        {
            borderNode = new BorderNode;
            QskSGNode::setNodeRole( borderNode, BorderRole );
        }

#ifdef ARC_BORDER_NODE
        borderNode->updateNode( arcRect, metricsArc, borderWidth, borderColor );
#else
        QPen pen( borderColor, borderWidth );
        pen.setCapStyle( Qt::FlatCap );

        borderNode->updateNode( path, QTransform(), pen );
#endif
    }
    else
    {
        delete borderNode;
        borderNode = nullptr;
    }

    qskUpdateChildren( this, ShadowRole, shadowNode );
    qskUpdateChildren( this, FillRole, fillNode );
    qskUpdateChildren( this, BorderRole, borderNode );
}
