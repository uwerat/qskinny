/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskMetaMethod.h"

#include <QObject>
#include <QCoreApplication>
#include <QThread>
#include <QSemaphore>
#include <QMetaMethod>
#include <QDebug>

QSK_QT_PRIVATE_BEGIN
#include <private/qobject_p.h>
QSK_QT_PRIVATE_END

static inline void qskInvokeMethodQueued( QObject* object,
    const QMetaObject* metaObject, ushort methodIndex,
    int nargs, int* types, void* args[], QSemaphore* semaphore = nullptr )
{
    constexpr QObject* sender = nullptr;
    constexpr int signalId = -1;

    const int methodOffset = metaObject->methodOffset();

    auto event = new QMetaCallEvent(
        methodOffset, methodIndex - methodOffset,
        metaObject->d.static_metacall,
        sender, signalId, nargs, types, args, semaphore );

    QCoreApplication::postEvent( object, event );
}

void QskMetaMethod::invoke( QObject* object,
    const QMetaMethod& method, void* args[],
    Qt::ConnectionType connectionType )
{
    auto metaObject = method.enclosingMetaObject();
    if ( metaObject == nullptr )
        return;

    const int methodIndex = method.methodIndex() - metaObject->methodOffset();
    invoke( object, metaObject, methodIndex, args, connectionType );
}

void QskMetaMethod::invoke( QObject* object,
    const QMetaObject* metaObject, int methodIndex, void* argv[],
    Qt::ConnectionType connectionType )
{
    if ( ( metaObject == nullptr ) || ( methodIndex < 0 )
        || ( methodIndex >= metaObject->methodCount() ) )
    {
        return;
    }

    QPointer< QObject > receiver( object );

    int invokeType = connectionType & 0x3;

    if ( invokeType == Qt::AutoConnection )
    {
        invokeType = ( object && object->thread() != QThread::currentThread() )
            ? Qt::QueuedConnection : Qt::DirectConnection;
    }

    switch( invokeType )
    {
        case Qt::DirectConnection:
        {
#if 1
            if ( receiver.isNull() )
                return; // do we really always need an object here ???
#endif

            const int index = methodIndex - metaObject->methodOffset();

            if ( metaObject->d.static_metacall )
            {
                metaObject->d.static_metacall( receiver,
                    QMetaObject::InvokeMetaMethod, index, argv );
            }
            else
            {
                QMetaObject::metacall( receiver,
                    QMetaObject::InvokeMetaMethod, index, argv );
            }
            break;
        }
        case Qt::BlockingQueuedConnection:
        {
            if ( receiver.isNull()
                || ( receiver->thread() == QThread::currentThread() ) )
            {
                // We would end up in a deadlock, better do nothing
                return;
            }

            QSemaphore semaphore;

            qskInvokeMethodQueued( receiver, metaObject,
                methodIndex, 0, nullptr, argv, &semaphore );

            semaphore.acquire();

            break;
        }
        case Qt::QueuedConnection:
        {
            if ( receiver == nullptr )
                return;

#if 1
            // should be doable without QMetaMethod. TODO ...
            const auto method = metaObject->method( methodIndex );
#endif
            const int argc = method.parameterCount() + 1;

            auto types = static_cast< int* >( malloc( argc * sizeof( int ) ) );
            auto arguments = static_cast< void** >( malloc( argc * sizeof( void* ) ) );

            /*
                The first one is the return type, one that is always
                invalid for Queued Connections.
             */

            types[0] = QMetaType::UnknownType;
            arguments[0] = nullptr;

            for ( int i = 1; i < argc; i++ )
            {
                if ( argv[i] == nullptr )
                {
                    Q_ASSERT( argv[i] != nullptr );
                    receiver = nullptr;
                    break;
                }

                types[i] = method.parameterType( i - 1 );
                arguments[i] = QMetaType::create( types[i], argv[i] );
            }

            if ( receiver.isNull() )
            {
                // object might have died in the meantime
                free( types );
                free( arguments );

                return;
            }

            qskInvokeMethodQueued( object,
                metaObject, methodIndex, argc, types, arguments );

            break;
        }
    }
}

QMetaMethod QskMetaMethod::method(
    const QMetaObject* metaObject, const char* methodName )
{
    if ( metaObject == nullptr || methodName == nullptr )
        return QMetaMethod();

    constexpr char signalIndicator = '0' + QSIGNAL_CODE;
    constexpr char slotIndicator = '0' + QSLOT_CODE;

    int index = -1;

    if( methodName[0] == signalIndicator )
    {
        auto signature =  QMetaObject::normalizedSignature( methodName + 1 );
        index = metaObject->indexOfSignal( signature );
    }
    else if ( methodName[0] == slotIndicator )
    {
        auto signature =  QMetaObject::normalizedSignature( methodName + 1 );
        index = metaObject->indexOfSlot( signature );
    }
    else
    {
        auto signature = QMetaObject::normalizedSignature( methodName );
        index = metaObject->indexOfMethod( signature );
    }

    return ( index >= 0 ) ? metaObject->method( index ) : QMetaMethod();
}

QMetaMethod QskMetaMethod::notifySignal(
    const QMetaObject* metaObject, const char* propertyName )
{
    if ( metaObject == nullptr || propertyName == nullptr )
        return QMetaMethod();

    const int propertyIndex = metaObject->indexOfProperty( propertyName );
    if ( propertyIndex )
    {
        const auto property = metaObject->property( propertyIndex );
        return property.notifySignal();
    }

    return QMetaMethod();
}
