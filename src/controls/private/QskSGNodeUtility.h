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
    inline Q_REQUIRED_RESULT Root* ensureNodes( QSGNode* root = nullptr )
    {
        if ( root == nullptr )
        {
            root = new Root;
        }

        if constexpr ( mode == Recursive )
        {
            QSGNode* current = root;
            (
                [ & ]( QSGNode* const child ) {
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