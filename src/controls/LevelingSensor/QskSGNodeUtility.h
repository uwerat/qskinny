#pragma once

#include <QSGNode>
#include <type_traits>

namespace QskSGNode
{
    using NodeType = QSGNode;

    template<typename ... Ts>
    struct seq;

    template<>
    struct seq<>
    {
        static void append(NodeType* root)
        {
        }
    };

    template<typename T>
    struct seq<T>
    {
        static void append(NodeType* root)
        {
            T::append(root);
        }
    };

    template<typename T, typename ... Ts>
    struct seq<T, Ts...>
    {
        static void append(NodeType* root)
        {
            T::append(root);
            seq<Ts...>::append(root);
        }
    };

    template<int N, typename ... Ts>
    struct par;

    template<int N, typename T>
    struct par<N,T>
    {
        static void append(NodeType* root)
        {
            const auto n = N;
            for (int i = 0; i < N; ++i)
            {
                root->appendChildNode(new T);
            }
        }
    };

    template<int N, typename T, typename U>
    struct par<N,T,U>
    {
        static void append(NodeType* root)
        {
            const auto n = N;
            for (int i = 0; i < N; ++i)
            {
                auto* const t = new T;
                U::append(t);
                root->appendChildNode(t);
            }
        }
    };

    template<typename ... Ts>
    struct ensure;

    template<typename Root>
    struct ensure<Root>
    {
        static Q_REQUIRED_RESULT Root* node(NodeType* root = nullptr)
        {
            if(root == nullptr)
            {
                root = new Root;
            }
            return static_cast<Root*>(root);
        }
    };

    template<typename Root, typename Append>
    struct ensure<Root, Append>
    {
        static Q_REQUIRED_RESULT Root* node(NodeType* root = nullptr)
        {
            if(root == nullptr)
            {
                root = new Root;
                Append::append(root);
            }
            return static_cast<Root*>(root);
        }
    };
}