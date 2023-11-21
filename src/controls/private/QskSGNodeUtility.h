/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SGNODE_UTILITY_H
#define QSK_SGNODE_UTILITY_H

#include <QSGNode>

namespace QskSGNode
{
    enum AppendMode
    {
        Sequential,
        Recursive
    };

    template< AppendMode mode, typename Root, typename... Children >
    Q_REQUIRED_RESULT inline Root* ensureNodes( QSGNode* root = nullptr )
    {
        if ( root == nullptr )
        {
            root = new Root;
        }

        if constexpr ( mode == Recursive )
        {
            QSGNode* current = root;
            Q_UNUSED( current )
            (
                [ &current ]( QSGNode* const child ) mutable {
                    current->appendChildNode( child );
                    current = child;
                }( new Children ),
                ... );
        }
        else
        {
            ( root->appendChildNode( new Children ), ... );
        }

        return static_cast< Root* >( root );
    }
}

#endif