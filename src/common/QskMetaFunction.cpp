/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGlobal.h"
#include "QskMetaFunction.h"

#include <QCoreApplication>
#include <QThread>
#include <QObject>
#include <QSemaphore>

QSK_QT_PRIVATE_BEGIN
#include <private/qobject_p.h>
QSK_QT_PRIVATE_END

static inline void qskInvokeFunctionQueued( QObject* object,
    QskMetaInvokable* invokable, int argc, int* types, void* argv[],
    QSemaphore* semaphore = nullptr )
{
    constexpr QObject* sender = nullptr;
    constexpr int signalId = 0;

    auto event = new QMetaCallEvent(
        invokable, sender, signalId, argc, types, argv, semaphore );

    QCoreApplication::postEvent( object, event );
}

QskMetaFunction::QskMetaFunction():
    m_invokable( nullptr ),
    m_parameterTypes( nullptr )
{
}

QskMetaFunction::QskMetaFunction( const QskMetaFunction& other ):
    m_invokable( other.m_invokable ),
    m_parameterTypes( other.m_parameterTypes )
{
    if ( m_invokable )
        m_invokable->ref();
}

QskMetaFunction::QskMetaFunction( QskMetaFunction&& other ):
    m_invokable( other.m_invokable ),
    m_parameterTypes( other.m_parameterTypes )
{
    other.m_invokable = nullptr;
}

QskMetaFunction::~QskMetaFunction()
{
    if ( m_invokable )
        m_invokable->destroyIfLastRef();
}

QskMetaFunction& QskMetaFunction::operator=( QskMetaFunction&& other )
{
    if ( m_invokable != other.m_invokable )
    {
        if ( m_invokable )
            m_invokable->destroyIfLastRef();

        m_invokable = other.m_invokable;
        other.m_invokable = nullptr;
    }

    m_parameterTypes = other.m_parameterTypes;

    return *this;
}

QskMetaFunction& QskMetaFunction::operator=( const QskMetaFunction& other )
{
    if ( m_invokable != other.m_invokable )
    {
        if ( m_invokable )
            m_invokable->destroyIfLastRef();

        m_invokable = other.m_invokable;

        if ( m_invokable )
            m_invokable->ref();
    }

    m_parameterTypes = other.m_parameterTypes;

    return *this;
}

void QskMetaFunction::init( QskMetaInvokable* invokable,
    const int* parameterTypes )
{
    m_invokable = invokable;
    m_parameterTypes = parameterTypes;
}

const int* QskMetaFunction::parameterTypes() const
{
    return m_parameterTypes;
}

size_t QskMetaFunction::parameterCount() const
{
    if ( m_parameterTypes )
    {
        for ( int i = 1; ; i++ )
        {
            if ( m_parameterTypes[ i ] == QMetaType::UnknownType )
                return i + 1; // including the return type
        }
    }

    return 1; // we always have a return type
}


QskMetaFunction::Type QskMetaFunction::functionType() const
{
    if ( m_invokable == nullptr )
        return Invalid;

    return static_cast< QskMetaFunction::Type >( m_invokable->typeInfo() );
}

void QskMetaFunction::invoke(
    QObject* object, void* argv[], Qt::ConnectionType connectionType )
{
    if ( m_invokable == nullptr )
        return;

    int invokeType = connectionType & 0x3;

    if ( invokeType == Qt::AutoConnection )
    {
        invokeType = ( object->thread() == QThread::currentThread() )
            ? Qt::DirectConnection : Qt::QueuedConnection;
    }
    else if ( invokeType == Qt::BlockingQueuedConnection )
    {
        if ( object->thread() == QThread::currentThread() )
        {
            // We would end up in a deadlock, better do nothing
            return;
        }
    }

    if ( invokeType == Qt::DirectConnection )
    {
        m_invokable->call( object, argv );
    }
    else
    {
        if ( object == nullptr )
        {
#if 1
            /*
                object might be deleted in another thread
                during this call - TODO ...
             */
#endif
            return;
        }

        const auto argc = parameterCount();

        auto types = static_cast< int* >( malloc( argc * sizeof( int ) ) );
        auto arguments = static_cast< void** >( malloc( argc * sizeof( void* ) ) );

        types[0] = QMetaType::UnknownType; // a return type is not possible
        arguments[0] = nullptr;

        for ( uint i = 1; i < argc; i++ )
        {
            if ( argv[i] == nullptr )
            {
                Q_ASSERT( arguments[i] != nullptr );

                free( types );
                free( arguments );

                return;
            }

            types[i] = m_parameterTypes[i - 1];
            arguments[i] = QMetaType::create( m_parameterTypes[i - 1], argv[i] );
        }

        if ( connectionType == Qt::QueuedConnection )
        {
            qskInvokeFunctionQueued( object, m_invokable, argc, types, arguments );
        }
        else // Qt::BlockingQueuedConnection ???
        {
            QSemaphore semaphore;

            qskInvokeFunctionQueued( object,
                m_invokable, argc, types, arguments, &semaphore );

            semaphore.acquire();
        }
    }
}
