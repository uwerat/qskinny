/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskShadedBoxNode.h"
#include "QskBoxFillNode.h"
#include "QskBoxShadowNode.h"
#include "QskBoxNode.h"
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

    auto node = static_cast< Node* > ( findChildNode( parentNode, ShadowRole ) );

    if ( node == nullptr )
    {
        node = new Node();
        setNodeRole( node, role );
    }

    return node;
}

static inline void qskInsertNode( QSGNode* parentNode, QSGNode* node )
{
    static const QVector< quint8 > roles = { ShadowRole, BoxRole, FillRole };

    QskSGNode::replaceChildNode( roles, QskSGNode::nodeRole( node ),
        parentNode, nullptr, node );
}

static inline bool qskIsBoxGradient( const QskGradient& gradient )
{
    if ( !gradient.isVisible() || gradient.isMonochrome() )
        return true;

    switch( gradient.type() )
    {
        case QskGradient::Linear:
        {
            auto dir = gradient.linearDirection();

            if ( dir.isTilted() )
            {
                // only diagonal from topLeft to bottomRight
                return ( dir.x1() == dir.x2() ) && ( dir.y1() == dir.y2() );
            }

            return true;
        }
        case QskGradient::Radial:
        case QskGradient::Conic:
        {
            return false;
        }
        default:
        {
            return true;
        }
    }
}

QskShadedBoxNode::QskShadedBoxNode()
{
}

QskShadedBoxNode::~QskShadedBoxNode()
{
}

void QskShadedBoxNode::setBoxData( const QRectF& rect,
    const QskBoxShapeMetrics& shape, const QskBoxBorderMetrics& borderMetrics,
    const QskBoxBorderColors& borderColors, const QskGradient& gradient,
    const QskShadowMetrics& shadowMetrics, const QColor& shadowColor )
{
    using namespace QskSGNode;

    QskBoxShadowNode* shadowNode = nullptr;
    QskBoxNode* boxNode = nullptr;
    QskBoxFillNode* fillNode = nullptr;

    if ( !shadowMetrics.isNull()
        && shadowColor.isValid() && shadowColor.alpha() != 0 )
    {
        shadowNode = qskNode< QskBoxShadowNode >( this, ShadowRole );
        shadowNode->setShadowData( shadowMetrics.shadowRect( rect ),
            shape, shadowMetrics.blurRadius(), shadowColor );
    }

    /*
        QskBoxNode supports vertical/horizontal/diagonal gradients only.
        If our gradient doesn't fall into this category we use a QskBoxFillNode.
        However the border is always done with a QskBoxNode
     */

    if ( qskIsBoxGradient( gradient ) )
    {
        boxNode = qskNode< QskBoxNode >( this, BoxRole );
        boxNode->setBoxData( rect, shape, borderMetrics, borderColors, gradient );
    }
    else
    {
        if ( !borderMetrics.isNull() && borderColors.isVisible() )
        {
            boxNode = qskNode< QskBoxNode >( this, BoxRole );
            boxNode->setBoxData( rect, shape, borderMetrics, borderColors, QskGradient() );
        }

        if ( gradient.isVisible() )
        {
            fillNode = qskNode< QskBoxFillNode >( this, FillRole );
            fillNode->updateNode( rect, shape, borderMetrics, gradient );
        }
    }

    qskUpdateChildren( this, ShadowRole, shadowNode );
    qskUpdateChildren( this, BoxRole, boxNode );
    qskUpdateChildren( this, FillRole, fillNode );
}
