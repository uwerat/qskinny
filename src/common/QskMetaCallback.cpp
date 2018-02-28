/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskMetaCallback.h"

#include <QObject>
#include <QCoreApplication>
#include <QThread>
#include <QSemaphore>

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
    m_method( new QMetaMethod( method ) ),
    m_type( MetaMethod ),
    m_connectionType( static_cast< ushort >( connectionType & ~Qt::UniqueConnection ) )
{
}

QskMetaCallback::QskMetaCallback( const QObject* object,
        const QskMetaFunction& function, Qt::ConnectionType connectionType ):
    m_object( const_cast< QObject* >( object ) ),
    m_function( new QskMetaFunction( function ) ),
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
            m_method = new QMetaMethod( *other.m_method );
            break;

        case MetaFunction:
            m_function = new QskMetaFunction( *other.m_function );
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

    if ( other.m_type != Invalid )
    {
        if ( other.m_type == MetaMethod )
            m_method = new QMetaMethod( *other.m_method );
        else
            m_function = new QskMetaFunction( *other.m_function );
    }

    return *this;
}

void QskMetaCallback::setConnectionType( Qt::ConnectionType connectionType )
{
    m_connectionType = connectionType;
}

void QskMetaCallback::reset()
{
    switch( m_type )
    {
        case MetaMethod:
            delete m_method;
            m_method = nullptr;
            break;

        case MetaFunction:
            delete m_function;
            m_function = nullptr;
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
            const int paramCount = m_method->parameterCount();

            paramTypes.reserve( paramCount );
            for ( int i = 0; i < paramCount; i++ )
                paramTypes += m_method->parameterType( i );

            break;
        }
        case MetaFunction:
        {
            auto types = m_function->parameterTypes();
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
            qskInvokeMethod( object, *m_method, args, connectionType() );
            break;
        }
        case MetaFunction:
        {
            m_function->invoke( object, args, connectionType() );
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
    if ( object == nullptr )
        return;

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
