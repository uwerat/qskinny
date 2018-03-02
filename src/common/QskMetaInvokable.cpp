/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskMetaInvokable.h"

namespace
{
    // to have access to the private section of QSlotObjectBase
    struct SlotObject
    {
        QAtomicInt ref;
        QskMetaInvokable::InvokeFunction invoke;
        const int* parameterTypes;
    };

    static_assert( sizeof( SlotObject ) == sizeof( QskMetaInvokable ),
        "Bad cast: QskMetaInvokable does not match" );
}

static inline void qskCallFunction( void (*function)(),
    void** args, const int* types )
{
    if ( types == nullptr || args == nullptr )
    {
        function();
        return;
    }
}

QskMetaInvokable* QskMetaInvokable::instance(
    InvokeFunction invoke, const int* parameterTypes, void** functor )
{
    /*
        In opposite to QObject::connect we share the Invokable for callbacks to the same
        function/functor - like f.e QQuickItem::update(). But we have to pay an extra static
        pointer inside of each callback instance.
     */
    QskMetaInvokable* invokable;
    void* args[] = { &invokable, functor };

    invoke( Find, nullptr, nullptr, args, nullptr );

    if ( invokable )
        invokable->ref();
    else
        invoke( Create, nullptr, nullptr, args, nullptr );

    invokable->m_parameterTypes = parameterTypes;

    return invokable;
}

int QskMetaInvokable::typeInfo() const
{
    auto that = const_cast< QskMetaInvokable* >( this );

    int value;

    reinterpret_cast< SlotObject* >( that )->invoke( TypeInfo, that,
        nullptr, reinterpret_cast< void** >( &value ), nullptr );

    return value;
}

int QskMetaInvokable::refCount() const
{
    auto that = const_cast< QskMetaInvokable* >( this );
    return reinterpret_cast< SlotObject* >( that )->ref.load();
}

QskMetaFunctionInvokable0::QskMetaFunctionInvokable0( Function function ):
    QskMetaInvokable( &invoke, nullptr ),
    m_function( function )
{
}

void QskMetaFunctionInvokable0::invoke(int which, QtPrivate::QSlotObjectBase* invokable,
    QObject*, void** args, bool* )
{
    switch ( which )
    {
        case Find:
        {
            *reinterpret_cast< void** >( args[0] ) = nullptr;
            break;
        }
        case Create:
        {
            *reinterpret_cast< void** >( args[0] ) = 
                new Invokable( *reinterpret_cast< Function* >( args[1] ) );

            break;
        }
        case Destroy:
        {
            delete static_cast< Invokable* >( invokable );
            break;
        }
        case Call:
        {
            auto invokable01 = static_cast< Invokable* >( invokable );
            qskCallFunction( invokable01->m_function,
                args, invokable01->parameterTypes() );

            break;
        }
        case TypeInfo:
        {
            *reinterpret_cast< int* >( args ) = 1; // QskMetaFunction::Function
            break;
        }
    }
}
