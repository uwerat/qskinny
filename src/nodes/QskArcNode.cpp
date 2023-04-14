/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskArcNode.h"
#include "QskArcRenderer.h"
#include "QskArcMetrics.h"
#include "QskMargins.h"
#include "QskGradient.h"
#include "QskGradientDirection.h"
#include "QskShapeNode.h"
#include "QskStrokeNode.h"
#include "QskSGNode.h"

#include <qpen.h>
#include <qpainterpath.h>

#define LINEAR_GRADIENT_HACK 1

#if LINEAR_GRADIENT_HACK

static inline QskGradient qskBuildGradient( QskGradient::Type type,
    const QRectF& rect, const QskArcMetrics& metrics,
    const QskGradientStops& stops )
{
    const auto center = rect.center();

    QskGradient gradient;
    gradient.setStretchMode( QskGradient::NoStretch );

    if ( type == QskGradient::Conic )
    {
        gradient.setConicDirection(
            center.x(), center.y(), metrics.startAngle() );

        gradient.setStops( stops );
    }
    else
    {
        gradient.setRadialDirection( center.x(), center.y(),
            rect.width(), rect.height() );

        {
            /*
                Trying to do what QGradient::LogicalMode does in
                the previous QPainter based implementation
             */

            const auto radius = 0.5 * qMin( rect.width(), rect.height() );
            const auto t = metrics.thickness() / radius;

            QskGradientStops scaledStops;
            scaledStops.reserve( stops.size() );

            for ( const auto& stop : stops )
            {
                const auto pos = 0.5 - t * ( 0.75 - stop.position() );
                scaledStops += QskGradientStop( pos, stop.color() );
            }

            gradient.setStops( scaledStops );
        }
    }

    return gradient;
}

#endif

static inline QskGradient qskEffectiveGradient(
    const QskGradient& gradient, const QRectF& rect,
    const QskArcMetrics& metrics )
{
    if ( !gradient.isMonochrome() )
    {
        if ( gradient.type() == QskGradient::Stops )
        {
            const QskConicDirection dir(
                rect.center(), metrics.startAngle() );
#if 0
            dir.setSpanAngle( metrics.spanAngle() ); // what is "expected" ??
#endif

            QskGradient g( gradient.stops() );
            g.setStretchMode( QskGradient::NoStretch );
            g.setConicDirection( dir );

            return g;
        }

#if LINEAR_GRADIENT_HACK
        if ( gradient.type() == QskGradient::Linear )
        {
            // to keep the iotdashboard working: to be removed

            const auto type = gradient.linearDirection().isHorizontal()
                ? QskGradient::Conic : QskGradient::Radial;

            return qskBuildGradient( type, rect, metrics, gradient.stops() );
        }
#endif
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

QskArcNode::QskArcNode()
{
}

QskArcNode::~QskArcNode()
{
}

void QskArcNode::setArcData( const QRectF& rect,
    const QskArcMetrics& arcMetrics, const QskGradient& fillGradient )
{
    setArcData( rect, arcMetrics, 0.0, QColor(), fillGradient );
}

void QskArcNode::setArcData( const QRectF& rect, const QskArcMetrics& arcMetrics,
    qreal borderWidth, const QColor borderColor, const QskGradient& fillGradient )
{
    enum NodeRole
    {
        FillRole,
        BorderRole
    };

    const auto metrics = qskEffectiveMetrics( arcMetrics, rect );
    const auto gradient = qskEffectiveGradient( fillGradient, rect, metrics );

    auto fillNode = static_cast< QskShapeNode* >(
        QskSGNode::findChildNode( this, FillRole ) );

    auto borderNode = static_cast< QskStrokeNode* >(
        QskSGNode::findChildNode( this, BorderRole ) );

    const auto arcRect = qskEffectiveRect( rect, borderWidth );
    if ( arcRect.isEmpty() )
    {
        delete fillNode;
        delete borderNode;

        return;
    }

    const auto path = QskArcRenderer::arcPath( arcRect, metrics );

    if ( gradient.isVisible() && !metrics.isNull() )
    {
        if ( fillNode == nullptr )
        {
            fillNode = new QskShapeNode;
            QskSGNode::setNodeRole( fillNode, FillRole );

            prependChildNode( fillNode );
        }

        fillNode->updateNode( path, QTransform(), arcRect, gradient );
    }
    else
    {
        delete fillNode;
    }

    if ( borderWidth > 0.0 && borderColor.alpha() > 0 )
    {
        if ( borderNode == nullptr )
        {
            borderNode = new QskStrokeNode;
            QskSGNode::setNodeRole( borderNode, BorderRole );

            appendChildNode( borderNode );
        }

        QPen pen( borderColor, borderWidth );
        pen.setCapStyle( Qt::FlatCap );
        
        borderNode->updateNode( path, QTransform(), pen );
    }
    else
    {
        delete borderNode;
    }
}
