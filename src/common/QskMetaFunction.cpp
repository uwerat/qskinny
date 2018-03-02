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
    m_invokable( nullptr )
{
}

QskMetaFunction::QskMetaFunction( QskMetaInvokable* invokable ):
    m_invokable( invokable )
{
    if ( m_invokable )
        m_invokable->ref();
}

QskMetaFunction::QskMetaFunction( const QskMetaFunction& other ):
    m_invokable( other.m_invokable )
{
    if ( m_invokable )
        m_invokable->ref();
}

QskMetaFunction::QskMetaFunction( QskMetaFunction&& other ):
    m_invokable( other.m_invokable )
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

    return *this;
}

size_t QskMetaFunction::parameterCount() const
{
    if ( auto types = parameterTypes() )
    {
        for ( int i = 1;; i++ )
        {
            if ( types[ i ] == QMetaType::UnknownType )
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
    // code is not thread safe - TODO ...

    QPointer< QObject > receiver( object );

    if ( m_invokable == nullptr )
        return;

    int invokeType = connectionType & 0x3;

    if ( invokeType == Qt::AutoConnection )
    {
        invokeType = ( receiver && receiver->thread() != QThread::currentThread() )
            ? Qt::QueuedConnection : Qt::DirectConnection;
    }

    switch( invokeType )
    {
        case Qt::DirectConnection:
        {
            m_invokable->call( receiver, argv );
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

            qskInvokeFunctionQueued( receiver, m_invokable,
                0, nullptr, argv, &semaphore );

            semaphore.acquire();

            break;
        }
        case Qt::QueuedConnection:
        {
            if ( receiver.isNull() )
            {
                return;
            }

            const auto argc = parameterCount();

            auto types = static_cast< int* >( malloc( argc * sizeof( int ) ) );
            auto arguments = static_cast< void** >( malloc( argc * sizeof( void* ) ) );

            types[0] = QMetaType::UnknownType; // a return type is not possible
            arguments[0] = nullptr;

            const int* parameterTypes = m_invokable->parameterTypes();
            for ( uint i = 1; i < argc; i++ )
            {
                if ( argv[i] == nullptr )
                {
                    Q_ASSERT( arguments[i] != nullptr );
                    receiver = nullptr;
                    break;
                }

                types[i] = parameterTypes[i - 1];
                arguments[i] = QMetaType::create( parameterTypes[i - 1], argv[i] );
            }

            if ( receiver.isNull() )
            {
                // object might have died in the meantime
                free( types );
                free( arguments );

                return;
            }

            qskInvokeFunctionQueued( object, m_invokable, argc, types, arguments );
            break;
        }
    }
}

#include "moc_QskMetaFunction.cpp"
