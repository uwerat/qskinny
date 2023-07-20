#pragma once

#include <QSGNode>

namespace QskSGNode
{    
    namespace detail
    {
        template< typename... Ts >
        struct append;

        template<>
        struct append<>
        {
            static void nodes( QSGNode* root )
            {
            }
        };

        template< typename Child, typename... Children >
        struct append< Child, Children... >
        {
            static void nodes( QSGNode* root )
            {
                auto* const child = new Child;
                root->appendChildNode( child );
                append< Children... >::nodes( child );
            }
        };
    }

    template< typename Root, typename... Children >
    inline Q_REQUIRED_RESULT Root* ensureNode( QSGNode* root = nullptr )
    {
        if ( root == nullptr )
        {
            root = new Root;
            detail::append< Children... >::nodes( root );
        }
        return static_cast< Root* >( root );
    }
}