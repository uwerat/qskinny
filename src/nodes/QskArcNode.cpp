/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskArcNode.h"
#include "QskArcMetrics.h"
#include "QskArcShadowNode.h"
#include "QskMargins.h"
#include "QskGradient.h"
#include "QskShapeNode.h"
#include "QskStrokeNode.h"
#include "QskSGNode.h"
#include "QskShadowMetrics.h"

#include <qpen.h>
#include <qpainterpath.h>

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

    auto fillNode = static_cast< QskShapeNode* >(
        QskSGNode::findChildNode( this, FillRole ) );

    auto borderNode = static_cast< QskStrokeNode* >(
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
    const auto isStrokeNodeVisible = borderWidth > 0.0 && borderColor.alpha() > 0;
    const auto isShadowNodeVisible = shadowColor.alpha() > 0.0 && isFillNodeVisible;

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
            fillNode = new QskShapeNode;
            QskSGNode::setNodeRole( fillNode, FillRole );
        }

        fillNode->updateNode( path, QTransform(), arcRect, gradient );
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
            borderNode = new QskStrokeNode;
            QskSGNode::setNodeRole( borderNode, BorderRole );
        }

        QPen pen( borderColor, borderWidth );
        pen.setCapStyle( Qt::FlatCap );

        borderNode->updateNode( path, QTransform(), pen );
    }
    else
    {
        delete borderNode;
        borderNode = nullptr;
    }

    qskUpdateChildren(this, ShadowRole, shadowNode);
    qskUpdateChildren(this, FillRole, fillNode);
    qskUpdateChildren(this, BorderRole, borderNode);
}
