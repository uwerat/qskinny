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

#include <QFile>
#include <QDebug>
#include <qmath.h>

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
    const qreal borderWidth, const QColor& borderColor, const QskGradient& fillGradient, const QColor& shadowColor, const QskShadowMetrics& shadowMetrics )
{
    enum NodeRole
    {
        ShadowRole,
        FillRole,
        BorderRole
    };

    const auto metrics = qskEffectiveMetrics( arcMetrics, rect );
    const auto gradient = qskEffectiveGradient( fillGradient, metrics );

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

    const auto path = metrics.painterPath( arcRect );

    if ( shadowColor.alpha() > 0.0 )
    {
        if ( shadowNode == nullptr ) 
        {
            shadowNode = new QskArcShadowNode;
            QskSGNode::setNodeRole( shadowNode, ShadowRole );            
        }
        
        shadowNode->update( arcRect, metrics, shadowColor, shadowMetrics, borderWidth );
    }
    else
    {
        delete shadowNode;
        shadowNode = nullptr;
    }

    if ( gradient.isVisible() && !metrics.isNull() )
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

    if ( borderWidth > 0.0 && borderColor.alpha() > 0 )
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
    
    const auto oldChildCount = this->childCount();    
    const auto newChildCount =
        ( shadowNode ? 1 : 0 ) + ( fillNode ? 1 : 0 ) + ( borderNode ? 1 : 0 );
     
    if ( oldChildCount != newChildCount ) 
    {
        removeAllChildNodes();
        
        for ( QSGNode* node : { ( QSGNode* ) shadowNode, ( QSGNode* ) fillNode, ( QSGNode* ) borderNode })
        {
            if ( node != nullptr ) 
            {
                appendChildNode( node );
            }
        }
    }
}
