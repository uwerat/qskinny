/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_META_CALL_H
#define QSK_META_CALL_H 1

#include "QskGlobal.h"
#include <QObject>

namespace QskMetaCall
{
    class Invokable;

    QSK_EXPORT void invoke( QObject* object,
        const QMetaMethod&, void* args[],
        Qt::ConnectionType = Qt::AutoConnection );

    QSK_EXPORT void invoke( QObject*,
        const Invokable&, int argc, const int argTypes[], void* args[],
        Qt::ConnectionType = Qt::AutoConnection );
}

namespace QskMetaCall
{
    using namespace QtPrivate;

    class QSK_EXPORT Invokable : public QSlotObjectBase
    {
    public:
        typedef void (* InvokeFunction)( int which, QSlotObjectBase*,
            QObject*, void**, bool* );

        enum { TypeInfo = NumOperations + 1 };

        int typeInfo() const;

    protected:
        explicit Invokable( InvokeFunction f ):
            QSlotObjectBase( f )
        {
        }
    };

    template< typename Func, typename Args, typename R >
    class FunctionInvokable : public Invokable
    {
    public:
        typedef FunctionPointer< Func > FuncType;

        explicit FunctionInvokable( Func f ):
            Invokable( &invoke ),
            function(f)
        {
        }

        static void invoke(int which, QSlotObjectBase* invokable,
            QObject* object, void** args, bool* )
        {
            auto f = static_cast< FunctionInvokable* >( invokable );

            switch ( which )
            {
                case Destroy:
                {
                    delete f;
                    break;
                }
                case Call:
                {
                    FuncType::template call< Args, R >( f->function, object, args );
                    break;
                }
                case TypeInfo:
                {
                    int* typeInfo = reinterpret_cast< int* >( args );
                    //*typeInfo = QskMetaFunction::Function;
                    *typeInfo = 1;

                    break;
                }
            }
        }

        Func function;
    };

    template< typename Func, typename Args, typename R >
    class MemberFunctionInvokable : public Invokable
    {
    public:
        explicit MemberFunctionInvokable( Func f ):
            Invokable( &invoke ),
            function(f)
        {
        }

        static void invoke( int which, QSlotObjectBase* invokable,
            QObject* object, void** args, bool* ret )
        {
            typedef FunctionPointer< Func > FuncType;

            auto f = static_cast< MemberFunctionInvokable* >( invokable );

            switch (which)
            {
                case Destroy:
                {
                    delete f;
                    break;
                }
                case Call:
                {
                    FuncType::template call< Args, R >(
                        f->function, static_cast< typename FuncType::Object* >( object ), args );

                    break;
                }
                case Compare:
                {
                    *ret = *reinterpret_cast< Func* >( args ) == f->function;
                    break;
                }
                case TypeInfo:
                {
                    int* typeInfo = reinterpret_cast< int* >( args );
                    *typeInfo = 0;
                    //*typeInfo = QskMetaFunction::Member;

                    break;
                }
            }
        }

    private:
        Func function;
    };

    template< typename Func, int N, typename Args, typename R >
    class FunctorInvokable : public Invokable
    {
    public:
        typedef Functor< Func, N > FuncType;

        explicit FunctorInvokable( Func f ):
            Invokable( &invoke ),
            function( std::move( f ) )
        {
        }

        static void invoke( int which, QSlotObjectBase* invokable,
            QObject* object, void** args, bool* )
        {
            auto f = static_cast< FunctorInvokable* >( invokable );

            switch (which)
            {
                case Destroy:
                {
                    delete f;
                    break;
                }
                case Call:
                {
                    FuncType::template call< Args, R >( f->function, object, args );
                    break;
                }
                case TypeInfo:
                {
                    int* typeInfo = reinterpret_cast< int* >( args );
                    *typeInfo = 2;
                    //*typeInfo = QskMetaFunction::Functor;

                    break;
                }
            }
        }

    private:
        Func function;
    };

    template< typename T >
    using IsMemberFunction = typename std::enable_if< FunctionPointer< T >::IsPointerToMemberFunction,
         std::true_type >::type;

    template< typename T >
    using IsFunction = typename std::enable_if< !FunctionPointer< T >::IsPointerToMemberFunction
        && FunctionPointer< T >::ArgumentCount >= 0, std::true_type >::type;

    template< typename T >
    using IsFunctor = typename std::enable_if< !FunctionPointer< T >::IsPointerToMemberFunction
        && FunctionPointer< T >::ArgumentCount == -1, std::true_type >::type;
}

#endif
