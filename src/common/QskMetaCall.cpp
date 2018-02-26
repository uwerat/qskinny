/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskMetaCall.h"
#include <QMetaMethod>
#include <QCoreApplication>
#include <QThread>
#include <QObject>
#include <QSemaphore>

#include <private/qobject_p.h>

namespace QskMetaCall
{
    inline Invokable::InvokeFunction invokeCall( const Invokable* invokable )
    {
        struct Data { QAtomicInt dummy; Invokable::InvokeFunction invoke; };
        return ( ( Data* )( invokable ) )->invoke;
    }

    int Invokable::typeInfo() const
    {
        int value;

        auto f = invokeCall( this );

        f( TypeInfo, const_cast< Invokable* >( this ),
            nullptr, reinterpret_cast< void** >( &value ), nullptr );

        return value;
    }
}

void QskMetaCall::invoke( QObject* object,
    const QMetaMethod& method, void* args[],
    Qt::ConnectionType connectionType )
{
    auto metaObject = method.enclosingMetaObject();

    const int methodOffset = metaObject->methodOffset();
    const int methodIndex  = method.methodIndex() - methodOffset;

    if ( connectionType == Qt::AutoConnection )
    {
        connectionType = ( object->thread() == QThread::currentThread() )
            ? Qt::DirectConnection : Qt::QueuedConnection;
    }

    if ( connectionType == Qt::DirectConnection )
    {
        if ( metaObject->d.static_metacall )
        {
            metaObject->d.static_metacall(object,
                QMetaObject::InvokeMetaMethod, methodIndex, args );
        }
        else
        {
            QMetaObject::metacall( object,
                QMetaObject::InvokeMetaMethod, methodIndex, args );
        }
    }
    else
    {
        const int paramCount = method.parameterCount();

        auto types = static_cast< int* >( malloc( paramCount * sizeof( int ) ) );

        types[0] = QMetaType::UnknownType; // return type

        for ( int i = 1; i < paramCount; i++ )
        {
            types[i] = method.parameterType( i );
            Q_ASSERT( args[i] != nullptr );
        }

        Q_ASSERT( args[paramCount] == nullptr );

        if ( connectionType == Qt::QueuedConnection )
        {
            QMetaCallEvent* event = new QMetaCallEvent(
                    methodOffset, methodIndex, metaObject->d.static_metacall,
                    nullptr, -1, paramCount + 1, types, args );

            QCoreApplication::postEvent(object, event );
        }
        else
        {
            QSemaphore semaphore;

            // what about argc + types ???
            auto event = new QMetaCallEvent(
                    methodOffset, methodIndex, metaObject->d.static_metacall,
                    nullptr, -1, 0, 0, args, &semaphore );

            QCoreApplication::postEvent( object, event );

            semaphore.acquire();
        }
    }
}

void QskMetaCall::invoke( QObject* object,
    const Invokable& invokable, void* args[],
    Qt::ConnectionType connectionType )
{
    //connectionType &= ~Qt::UniqueConnection;

    auto invokablePtr = const_cast< Invokable* >( &invokable );

    if ( connectionType == Qt::AutoConnection )
    {
        connectionType = ( object->thread() == QThread::currentThread() )
            ? Qt::DirectConnection : Qt::QueuedConnection;
    }

    if ( connectionType == Qt::DirectConnection )
    {
        invokablePtr->call( object, args );
    }
    else
    {
        if ( connectionType == Qt::QueuedConnection )
        {
            auto event = new QMetaCallEvent(
                invokablePtr, nullptr, 0, 0, nullptr, args, nullptr );

            QCoreApplication::postEvent( object, event );
        }
        else // Qt::BlockingQueuedConnection ???
        {
            // ....
        }
    }
}
