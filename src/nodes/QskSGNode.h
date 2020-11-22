/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SG_NODE_H
#define QSK_SG_NODE_H

#include "QskGlobal.h"
#include <qsgnode.h>

namespace QskSGNode
{
    enum Role : quint8
    {
        FirstReservedRole = 0xff - 10,

        DebugRole = 0xff - 2,
        BackgroundRole,

        NoRole
    };

    inline QSGNode::Flags nodeRoleFlags( quint8 role )
    {
        return static_cast< QSGNode::Flags >( ( role + 1 ) << 8 );
    }

    inline quint8 nodeRole( QSGNode::Flags flags )
    {
        return static_cast< quint8 >( ( ( flags & 0x0ffff ) >> 8 ) - 1 );
    }

    inline quint8 nodeRole( const QSGNode* node )
    {
        return node ? nodeRole( node->flags() ) : 0xff;
    }

    inline void setNodeRole( QSGNode* node, quint8 role )
    {
        if ( node )
            node->setFlags( node->flags() | nodeRoleFlags( role ) );
    }

    QSK_EXPORT QSGNode* findChildNode( QSGNode* parent, quint8 role );

    // nodeRoles: sort order
    QSK_EXPORT void replaceChildNode(
        const QVector< quint8 >& roles, quint8 role,
        QSGNode* parentNode, QSGNode* oldNode, QSGNode* newNode );

    // without child
    QSK_EXPORT void removeAllChildNodesAfter( QSGNode* parent, QSGNode* child );

    // including child
    QSK_EXPORT void removeAllChildNodesFrom( QSGNode* parent, QSGNode* child );

    template< typename Node >
    inline Node* createNode( quint8 role )
    {
        auto node = new Node();
        setNodeRole( node, role );

        return node;
    }

    template< typename Node >
    inline Node* appendChildNode( QSGNode* parent, quint8 role )
    {
        auto node = createNode< Node >( role );
        parent->appendChildNode( node );

        return node;
    }
}

#endif
