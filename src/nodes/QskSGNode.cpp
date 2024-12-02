/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskSGNode.h"

static inline void qskRemoveChildNode( QSGNode* parent, QSGNode* child )
{
    parent->removeChildNode( child );

    if ( child->flags() & QSGNode::OwnedByParent )
        delete child;

}

static inline void qskRemoveAllChildNodesAfter( QSGNode* parent, QSGNode* child )
{
    while ( auto sibling = parent->lastChild() )
    {
        if ( sibling == child )
            return;

        qskRemoveChildNode( parent, sibling );
    }
}

static void qskInsertChildSorted( QSGNode* parent, QSGNode* child,
    const QVector< quint8 >& roles )
{
    QSGNode* sibling = nullptr;

    if ( parent->childCount() > 0 )
    {
        using namespace QskSGNode;

        const int nodePos = roles.indexOf( nodeRole( child ) );

        // in most cases we are appending, so let's start at the end

        for ( auto childNode = parent->lastChild();
            childNode != nullptr; childNode = childNode->previousSibling() )
        {
            const auto childNodeRole = nodeRole( childNode );
            if ( childNodeRole == BackgroundRole )
            {
                sibling = childNode;
            }
            else
            {
                /*
                   Imperformant implementation, but as the number of roles is
                   usually < 5 we don't introduce some sort of support for faster lookups
                 */

                const int index = roles.indexOf( nodeRole( childNode ) );
                if ( index >= 0 && index < nodePos )
                    sibling = childNode;
            }

            if ( sibling != nullptr )
                break;
        }
    }

    if ( sibling )
        parent->insertChildNodeAfter( child, sibling );
    else
        parent->prependChildNode( child );
}

void QskSGNode::setParentNode( QSGNode* node, QSGNode* parent )
{
    if ( node )
    {
        auto oldParent = node->parent();

        if ( oldParent != parent )
        {
            if ( oldParent )
                oldParent->removeChildNode( node );

            if ( parent )
                parent->appendChildNode( node );
        }
    }
}

QSGNode* QskSGNode::findChildNode( QSGNode* parent, quint8 role )
{
    if ( parent )
    {
        auto node = parent->firstChild();
        while ( node )
        {
            if ( nodeRole( node ) == role )
                return node;

            node = node->nextSibling();
        }
    }

    return nullptr;
}

bool QskSGNode::removeChildNode( QSGNode* parent, quint8 role )
{
    if ( auto node = findChildNode( parent, role ) )
    {
        qskRemoveChildNode( parent, node );
        return true;
    }

    return false;
}

void QskSGNode::removeAllChildNodesAfter( QSGNode* parent, QSGNode* child )
{
    if ( parent && child && child->parent() == parent )
        qskRemoveAllChildNodesAfter( parent, child );
}

void QskSGNode::removeAllChildNodesFrom( QSGNode* parent, QSGNode* child )
{
    if ( parent && child && child->parent() == parent )
    {
        qskRemoveAllChildNodesAfter( parent, child );
        qskRemoveChildNode( parent, child );
    }
}

void QskSGNode::replaceChildNode(
    const QVector< quint8 >& roles, quint8 role,
    QSGNode* parentNode, QSGNode* oldNode, QSGNode* newNode )
{
    if ( newNode && newNode->parent() != parentNode )
    {
        setNodeRole( newNode, role );

        switch ( role )
        {
            case BackgroundRole:
            {
                parentNode->prependChildNode( newNode );
                break;
            }
            case DebugRole:
            {
                auto firstNode = parentNode->firstChild();

                if ( firstNode && ( nodeRole( firstNode ) == BackgroundRole ) )
                    parentNode->insertChildNodeAfter( newNode, firstNode );
                else
                    parentNode->prependChildNode( newNode );

                break;
            }
            default:
            {
                qskInsertChildSorted( parentNode, newNode, roles );
            }
        }
    }

    if ( oldNode && oldNode != newNode )
    {
        parentNode->removeChildNode( oldNode );
        if ( oldNode->flags() & QSGNode::OwnedByParent )
            delete oldNode;
    }
}

void QskSGNode::resetGeometry( QSGGeometryNode* node )
{
    if ( node )
    {
        if ( auto g = node->geometry() )
        {
            const bool hasVertices = g->vertexCount() > 0;
            const bool hasIndexes = g->indexCount() > 0;

            if ( hasVertices || hasIndexes )
            {
                g->allocate( 0, 0 );

                if ( hasVertices )
                    g->markVertexDataDirty();

                if ( hasIndexes )
                    g->markIndexDataDirty();

                node->markDirty( QSGNode::DirtyGeometry );
            }
        }
    }
}
