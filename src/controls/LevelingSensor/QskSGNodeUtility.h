#pragma once

#include <QSGNode>

namespace QskSGNode
{    
    enum AppendMode : bool {
        Sequential = false,
        Recursive = true
    };

    namespace detail
    {
        template< AppendMode Mode, typename... Ts >
        struct append;

        template<>
        struct append<AppendMode::Recursive>
        {
            static void nodes( QSGNode* root )
            {
            }
        };   
        
        template<>
        struct append<AppendMode::Sequential>
        {
            static void nodes( QSGNode* root )
            {
            }
        };

        template< typename Child, typename... Children >
        struct append< AppendMode::Recursive, Child, Children... >
        {
            static void nodes( QSGNode* root )
            {
                auto* const child = new Child;
                root->appendChildNode( child );
                append< AppendMode::Recursive, Children... >::nodes( child );
            }
        };

        template< typename Child, typename... Children >
        struct append< AppendMode::Sequential, Child, Children... >
        {
            static void nodes( QSGNode* root )
            {
                auto* const child = new Child;
                root->appendChildNode( child );
                append< AppendMode::Sequential, Children... >::nodes( root );
            }
        };
    }

    template< AppendMode mode, typename Root, typename... Children >
    inline Q_REQUIRED_RESULT Root* ensureNodes( QSGNode* root = nullptr )
    {
        if ( root == nullptr )
        {
            root = new std::remove_const_t<Root>();
            detail::append< mode, Children... >::nodes( root );
        }
        return static_cast<Root*>(root); 
    }
}