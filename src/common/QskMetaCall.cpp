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

namespace
{
    // to get access to the private section of QSlotObjectBase
    struct SlotObject
    {
        QAtomicInt ref;
        QskMetaCall::Invokable::InvokeFunction invoke;
    };
}

namespace QskMetaCall
{
    inline Invokable::InvokeFunction invokeCall( const Invokable* invokable )
    {
        return ( ( SlotObject* )( invokable ) )->invoke;
    }

    int Invokable::typeInfo() const
    {
        int value;

        auto f = invokeCall( this );

        f( TypeInfo, const_cast< Invokable* >( this ),
            nullptr, reinterpret_cast< void** >( &value ), nullptr );

        return value;
    }

    int Invokable::refCount() const
    {
        return ( ( SlotObject* )( this ) )->ref.load();
    }
}

namespace
{
    using namespace QskMetaCall;

    class FunctionCallEvent : public QMetaCallEvent
    {
    public:
        FunctionCallEvent( Invokable* invokable,
                int nargs, int* types, void* args[], QSemaphore* semaphore = nullptr ):
            QMetaCallEvent( invokable, nullptr, 0, nargs, types, args, semaphore ),
            m_invokable ( invokable )
        {
        }

        virtual ~FunctionCallEvent()
        {
        }

    private:
        Invokable* m_invokable;
    };

    class MethodCallEvent : public QMetaCallEvent
    {
    public:
        MethodCallEvent( const QMetaObject* mo, ushort methodIndex,
                int nargs, int* types, void* args[], QSemaphore* semaphore = nullptr ):
            QMetaCallEvent(
                mo->methodOffset(), methodIndex, mo->d.static_metacall,
                nullptr, -1, nargs, types, args, semaphore )
        {
        }

        virtual ~MethodCallEvent()
        {
        }
    };
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
        auto arguments = static_cast< void** >( malloc( paramCount * sizeof( void* ) ) );

        types[0] = QMetaType::UnknownType; // a return type is not possible
        arguments[0] = nullptr;

        for ( int i = 1; i < paramCount; i++ )
        {
            if ( arguments[i] == nullptr )
            {
                Q_ASSERT( arguments[i] != nullptr );

                free( types );
                free( arguments );

                return;
            }

            types[i] = method.parameterType( i );
            arguments[i] = args[i - 1];
        }

        Q_ASSERT( args[paramCount] == nullptr );

        if ( connectionType == Qt::QueuedConnection )
        {
            auto event = new MethodCallEvent(
                metaObject, methodIndex, paramCount + 1, types, args );

            QCoreApplication::postEvent(object, event );
        }
        else
        {
            QSemaphore semaphore;

            auto event = new MethodCallEvent(
                metaObject, methodIndex, paramCount + 1, types, args, &semaphore );

            QCoreApplication::postEvent( object, event );

            semaphore.acquire();
        }
    }
}

void QskMetaCall::invoke( QObject* object,
    const Invokable& invokable, int argc, const int argTypes[], void* argv[],
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
        invokablePtr->call( object, argv );
    }
    else
    {
        auto types = static_cast< int* >( malloc( argc * sizeof( int ) ) );
        auto arguments = static_cast< void** >( malloc( argc * sizeof( void* ) ) );

        types[0] = QMetaType::UnknownType; // a return type is not possible
        arguments[0] = nullptr;

        for ( int i = 1; i < argc; i++ )
        {
            if ( argv[i] == nullptr )
            {
                Q_ASSERT( arguments[i] != nullptr );

                free( types );
                free( arguments );

                return;
            }

            types[i] = argTypes[i - 1];
            arguments[i] = QMetaType::create( argTypes[i - 1], argv[i] );
        }

        if ( connectionType == Qt::QueuedConnection )
        {
            auto event = new FunctionCallEvent(
                invokablePtr, argc, types, arguments, nullptr );

            QCoreApplication::postEvent( object, event );
        }
        else // Qt::BlockingQueuedConnection ???
        {
            // ....
        }
    }
}
