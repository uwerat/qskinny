/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskMetaCallback.h"
#include <QCoreApplication>
#include <QThread>
#include <QObject>
#include <QSemaphore>

#include <private/qobject_p.h>

static void qskInvoke( QObject* object,
    const QMetaMethod& method, void* args[], Qt::ConnectionType connectionType )
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

QskMetaCallback::QskMetaCallback( const QObject* object,
    const QMetaMethod& method, Qt::ConnectionType connectionType ):
    m_object( const_cast< QObject* >( object ) ),
    m_method( method ),
    m_type( MetaMethod ),
    m_connectionType( static_cast< ushort >( connectionType & ~Qt::UniqueConnection ) )
{
}

QskMetaCallback::QskMetaCallback( const QObject* object,
        const QskMetaFunction& function, Qt::ConnectionType connectionType ):
    m_object( const_cast< QObject* >( object ) ),
    m_function( function ),
    m_type( MetaFunction ),
    m_connectionType( static_cast< ushort >( connectionType & ~Qt::UniqueConnection ) )
{
}

QskMetaCallback::QskMetaCallback( const QskMetaCallback& other ):
    m_object( other.m_object ),
    m_function(),
    m_type( Invalid ),
    m_connectionType( other.m_connectionType )
{
    if ( other.m_type != m_type )
    {
        reset();
        m_type = other.m_type;
    }

    switch( m_type )
    {
        case MetaMethod:
            m_method = other.m_method;
            break;

        case MetaFunction:
            m_function = other.m_function;
            break;

        default:
            break;
    }
}

QskMetaCallback::~QskMetaCallback()
{
    reset();
}

QskMetaCallback& QskMetaCallback::operator=( const QskMetaCallback& other )
{
    m_object = other.m_object;
    m_connectionType = other.m_connectionType;

    if ( other.m_type != m_type )
    {
        reset();
        m_type = other.m_type;
    }

    switch( m_type )
    {
        case MetaMethod:
            m_method = other.m_method;
            break;

        case MetaFunction:
            m_function = other.m_function;
            break;

        default:
            break;
    }

    return *this;
}

void QskMetaCallback::reset()
{
    switch( m_type )
    {
        case MetaMethod:
            m_method.~QMetaMethod();
            break;

        case MetaFunction:
            m_function.~QskMetaFunction();
            break;

        default:
            break;
    }

    m_type = Invalid;
}

QVector< int > QskMetaCallback::parameterTypes() const
{
    QVector< int > paramTypes;

    switch( m_type )
    {
        case MetaMethod:
        {
            const int paramCount = m_method.parameterCount();

            paramTypes.reserve( paramCount );
            for ( int i = 0; i < paramCount; i++ )
                paramTypes += m_method.parameterType( i );

            break;
        }
        case MetaFunction:
        {
            auto types = m_function.parameterTypes();
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
    auto object = const_cast< QObject* >( m_object.data() );

    switch( m_type )
    {
        case MetaMethod:
        {
            if ( object )
                qskInvoke( object, m_method, args, connectionType() );
            break;
        }
        case MetaFunction:
        {
            m_function.invoke( object, args, connectionType() );
            break;
        }

        default:
            break;
    }
}
