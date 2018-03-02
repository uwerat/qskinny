/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskMetaCallback.h"
#include "QskMetaFunction.h"

#include <QObject>
#include <QCoreApplication>
#include <QThread>
#include <QSemaphore>
#include <QMetaMethod>

QSK_QT_PRIVATE_BEGIN
#include <private/qobject_p.h>
QSK_QT_PRIVATE_END

static inline void qskInvokeMethodQueued( QObject* object,
    const QMetaObject* metaObject, ushort methodIndex,
    int nargs, int* types, void* args[], QSemaphore* semaphore = nullptr )
{
    constexpr QObject* sender = nullptr;
    constexpr int signalId = -1;

    auto event = new QMetaCallEvent(
        metaObject->methodOffset(), methodIndex, metaObject->d.static_metacall,
        sender, signalId, nargs, types, args, semaphore );

    QCoreApplication::postEvent( object, event );
}

QskMetaCallback::QskMetaCallback( const QObject* object,
        const QMetaMethod& method, Qt::ConnectionType connectionType ):
    m_object( const_cast< QObject* >( object ) ),
    m_methodData { method.enclosingMetaObject(), method.methodIndex()  },
    m_type( MetaMethod ),
    m_hasObject( object != nullptr ),
    m_connectionType( static_cast< ushort >( connectionType & 0x3 ) )
{
}

QskMetaCallback::QskMetaCallback( const QObject* object,
        const QskMetaFunction& function, Qt::ConnectionType connectionType ):
    m_object( const_cast< QObject* >( object ) ),
    m_functionData { function.invokable(), function.parameterTypes() },
    m_type( MetaFunction ),
    m_hasObject( object != nullptr ),
    m_connectionType( static_cast< ushort >( connectionType & ~Qt::UniqueConnection ) )
{
    if ( m_functionData.invokable )
        m_functionData.invokable->ref();
}

QskMetaCallback::QskMetaCallback( const QskMetaCallback& other ):
    m_object( other.m_object ),
    m_type( other.m_type ),
    m_hasObject( other.m_hasObject ),
    m_connectionType( other.m_connectionType )
{
    switch( m_type )
    {
        case MetaMethod:
        {
            m_methodData.metaObject = other.m_methodData.metaObject;
            m_methodData.methodIndex = other.m_methodData.methodIndex;

            break;
        }
        case MetaFunction:
        {
            m_functionData.invokable = other.m_functionData.invokable;
            if ( m_functionData.invokable )
                m_functionData.invokable->ref();

            m_functionData.parameterTypes = other.m_functionData.parameterTypes;
            break;
        }

        default:
            break;
    }
}

QskMetaCallback::~QskMetaCallback()
{
    if ( ( m_type == MetaFunction ) && m_functionData.invokable )
        m_functionData.invokable->destroyIfLastRef();
}

QskMetaCallback& QskMetaCallback::operator=( const QskMetaCallback& other )
{
    m_object = other.m_object;
    m_hasObject = other.m_hasObject;

    m_connectionType = other.m_connectionType;

    switch( other.m_type )
    {
        case MetaMethod:
        {
            if ( m_type == MetaFunction && m_functionData.invokable )
                m_functionData.invokable->destroyIfLastRef();

            m_methodData.metaObject = other.m_methodData.metaObject;
            m_methodData.methodIndex = other.m_methodData.methodIndex;

            break;
        }
        case MetaFunction:
        {
            if ( ( m_type == MetaFunction ) && m_functionData.invokable )
                m_functionData.invokable->destroyIfLastRef();

            m_functionData.invokable = other.m_functionData.invokable;

            if ( m_functionData.invokable )
                m_functionData.invokable->ref();

            m_functionData.parameterTypes = other.m_functionData.parameterTypes;
            break;
        }

        default:
            if ( ( m_type == MetaFunction ) && m_functionData.invokable )
                m_functionData.invokable->destroyIfLastRef();
    }

    m_type = other.m_type;

    return *this;
}

bool QskMetaCallback::isValid() const
{
    if ( m_hasObject && m_object.isNull() )
        return false;

    switch( m_type )
    {
        case MetaMethod:
        {
            const auto& d = m_methodData;
            if ( d.metaObject && ( d.methodIndex >= 0 )
                && ( d.methodIndex < d.metaObject->methodCount() ) )
            {
                return true;
            }

            return false;
        }

        case MetaFunction:
        {
            return m_functionData.invokable != nullptr;
        }

        default:
            return false;
    }
}

void QskMetaCallback::setConnectionType( Qt::ConnectionType connectionType )
{
    m_connectionType = connectionType;
}

void QskMetaCallback::reset()
{
    m_object = nullptr;
    m_hasObject = false;

    if ( m_type == MetaFunction && m_functionData.invokable )
        m_functionData.invokable->destroyIfLastRef();

    m_functionData = { nullptr, nullptr }; // for the debugger
    m_type = Invalid;
}

QVector< int > QskMetaCallback::parameterTypes() const
{
    QVector< int > paramTypes;

    switch( m_type )
    {
        case MetaMethod:
        {
            const auto& d = m_methodData;
            if ( m_methodData.metaObject )
            {
#if 1
                // should be doable without QMetaMethod. TODO ...
                const auto method = d.metaObject->method( d.methodIndex );
#endif
                const int paramCount = method.parameterCount();

                paramTypes.reserve( paramCount );
                for ( int i = 0; i < paramCount; i++ )
                    paramTypes += method.parameterType( i );
            }

            break;
        }
        case MetaFunction:
        {
            auto types = m_functionData.parameterTypes;
            if ( types )
            {
                while ( *types )
                    paramTypes += *types++;
            }
            break;
        }
        default:
            break;
    }

    return paramTypes;
}


void QskMetaCallback::invoke( void* args[] )
{
    if ( !isValid() )
        return;

    auto object = const_cast< QObject* >( m_object.data() );

    switch( m_type )
    {
        case MetaMethod:
        {
            qskInvokeMethod( object, m_methodData.metaObject,
                m_methodData.methodIndex, args, connectionType() );

            break;
        }
        case MetaFunction:
        {
            QskMetaFunction function( m_functionData.invokable );
            function.invoke( object, args, connectionType() );

            break;
        }

        default:
            break;
    }
}

void qskInvokeMethod( QObject* object,
    const QMetaMethod& method, void* args[],
    Qt::ConnectionType connectionType )
{
    auto metaObject = method.enclosingMetaObject();
    if ( metaObject == nullptr )
        return;

    const int methodIndex = method.methodIndex() - metaObject->methodOffset();
    qskInvokeMethod( object, metaObject, methodIndex, args, connectionType );
}

void qskInvokeMethod( QObject* object,
    const QMetaObject* metaObject, int methodIndex, void* args[],
    Qt::ConnectionType connectionType )
{
    if ( ( metaObject == nullptr ) || ( methodIndex < 0 )
        || ( methodIndex > metaObject->methodCount() ) )
    {
        return;
    }

    int invokeType = connectionType & 0x3;

    if ( invokeType == Qt::AutoConnection )
    {
        invokeType = ( object && object->thread() != QThread::currentThread() )
            ? Qt::QueuedConnection : Qt::DirectConnection;
    }
    else if ( invokeType == Qt::BlockingQueuedConnection )
    {
        if ( ( object == nullptr ) || object->thread() == QThread::currentThread() )
        {
            // We would end up in a deadlock, better do nothing
            return;
        }
    }

    if ( invokeType == Qt::DirectConnection )
    {
#if 1
        if ( object == nullptr )
            return; // do we really need an object here ???
#endif

        if ( metaObject->d.static_metacall )
        {
            metaObject->d.static_metacall( object,
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
        if ( object == nullptr )
            return;

#if 1
        // should be doable without QMetaMethod. TODO ...
        const auto method = metaObject->method( methodIndex );
#endif
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
            qskInvokeMethodQueued( object,
                metaObject, methodIndex, paramCount + 1, types, args );
        }
        else
        {
            QSemaphore semaphore;

            qskInvokeMethodQueued( object,
                metaObject, methodIndex, paramCount + 1, types, args, &semaphore );

            semaphore.acquire();
        }
    }
}
