/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTreeNode.h"

static constexpr auto extraFlag =
    static_cast< QSGNode::Flag >( QSGNode::IsVisitableNode << 1 );

static inline QSGNode* qskCheckedNode( const QSGNode* node, QSGNode::NodeType type )
{
    return node && ( node->type() == type ) && ( node->flags() & extraFlag )
        ? const_cast< QSGNode* >( node ) : nullptr;
}

QskTreeNode::QskTreeNode()
{
    setFlag( extraFlag, true );
}

QskTreeNode::QskTreeNode( QSGNodePrivate& d )
    : QSGNode( d, QSGNode::BasicNodeType )
{
    setFlag( extraFlag, true );
}

QskTreeNode::~QskTreeNode()
{
}

void QskTreeNode::setSubtreeBlocked( bool on, bool notify )
{
    if ( on == m_isBlocked )
        return;

    m_isBlocked = on;

    if ( notify )
        markDirty( DirtySubtreeBlocked );
}

bool QskTreeNode::isSubtreeBlocked() const
{
    return m_isBlocked;
}

QskTreeNode* qskTreeNodeCast( QSGNode* node )
{
    return static_cast< QskTreeNode* >(
        qskCheckedNode( node, QSGNode::BasicNodeType ) );
}

const QskTreeNode* qskTreeNodeCast( const QSGNode* node )
{
    return static_cast< QskTreeNode* >(
        qskCheckedNode( node, QSGNode::BasicNodeType ) );
}


// == QskItemNode

QskItemNode::QskItemNode()
{
    setFlag( extraFlag, true );
}

QskItemNode::~QskItemNode()
{
}

void QskItemNode::setSubtreeBlocked( bool on, bool notify )
{
    if ( on == m_isBlocked )
        return;

    m_isBlocked = on;

    if ( notify )
        markDirty( DirtySubtreeBlocked );
}

bool QskItemNode::isSubtreeBlocked() const
{
    return m_isBlocked;
}

QskItemNode* qskItemNodeCast( QSGNode* node )
{
    return static_cast< QskItemNode* >(
        qskCheckedNode( node, QSGNode::TransformNodeType ) );
}

const QskItemNode* qskItemNodeCast( const QSGNode* node )
{
    return static_cast< QskItemNode* >(
        qskCheckedNode( node, QSGNode::TransformNodeType ) );
}

bool qskIsBlockableNode( const QSGNode* node )
{
    switch( node->type() )
    {
        case QSGNode::BasicNodeType:
        case QSGNode::TransformNodeType:
            return node->flags() & extraFlag;

        default:
            return false;
    }
}

bool qskTryBlockNode( QSGNode* node, bool on, bool notify )
{
    if ( node && ( node->flags() & extraFlag ) )
    {
        if ( node->type() == QSGNode::BasicNodeType )
        {
            static_cast< QskTreeNode* >( node )->setSubtreeBlocked( on, notify );
            return true;
        }

        if ( node->type() == QSGNode::TransformNodeType )
        {
            static_cast< QskItemNode* >( node )->setSubtreeBlocked( on, notify );
            return true;
        }
    }

    return false;
}

void qskTryBlockTree( QSGNode* node, bool on, bool notify )
{
    if ( qskTryBlockNode( node, on, notify ) )
        return;

    for ( auto child = node->firstChild();
        child != nullptr; child = child->nextSibling() )
    {
        qskTryBlockTree( child, on, notify );
    }
}

void qskTryBlockTrailingNodes(
    QSGNode* node, const QSGNode* ancestorNode, bool on, bool notify )
{
    qskTryBlockTree( node, on, notify );

    for ( auto sibling = node->nextSibling();
        sibling != nullptr; sibling = sibling->nextSibling() )
    {
        qskTryBlockTree( sibling, on, notify );
    }

    if ( node != ancestorNode )
    {
        if ( auto upperNode = node->parent() )
        {
            upperNode = upperNode->nextSibling();
            if ( upperNode )
                qskTryBlockTrailingNodes( upperNode, ancestorNode, on, notify );
        }
    }
}
