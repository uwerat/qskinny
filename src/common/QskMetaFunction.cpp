/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGlobal.h"
#include "QskMetaFunction.h"

#include <qcoreapplication.h>
#include <qobject.h>
#include <qsemaphore.h>
#include <qthread.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qobject_p.h>
QSK_QT_PRIVATE_END

static inline void qskInvokeFunctionQueued( QObject* object,
    QskMetaFunction::FunctionCall* functionCall, int argc, int* types, void* argv[],
    QSemaphore* semaphore = nullptr )
{
    constexpr QObject* sender = nullptr;
    constexpr int signalId = 0;

    auto event = new QMetaCallEvent(
        functionCall, sender, signalId, argc, types, argv, semaphore );

    QCoreApplication::postEvent( object, event );
}

namespace
{
    using FunctionCall = QskMetaFunction::FunctionCall;

    // to have access to the private section of QSlotObjectBase
    struct SlotObject
    {
        QAtomicInt ref;
        FunctionCall::InvokeFunction invoke;
        const int* parameterTypes;
    };

    static_assert( sizeof( SlotObject ) == sizeof( FunctionCall ),
        "Bad cast: QskMetaFunction does not match" );
}

int QskMetaFunction::FunctionCall::typeInfo() const
{
    auto that = const_cast< FunctionCall* >( this );

    int value;

    reinterpret_cast< SlotObject* >( that )->invoke(
        TypeInfo, that, nullptr, reinterpret_cast< void** >( &value ), nullptr );

    return value;
}

int QskMetaFunction::FunctionCall::refCount() const
{
    auto that = const_cast< FunctionCall* >( this );
    return reinterpret_cast< SlotObject* >( that )->ref.load();
}

QskMetaFunction::QskMetaFunction()
    : m_functionCall( nullptr )
{
}

QskMetaFunction::QskMetaFunction( FunctionCall* functionCall )
    : m_functionCall( functionCall )
{
    if ( m_functionCall )
        m_functionCall->ref();
}

QskMetaFunction::QskMetaFunction( const QskMetaFunction& other )
    : m_functionCall( other.m_functionCall )
{
    if ( m_functionCall )
        m_functionCall->ref();
}

QskMetaFunction::QskMetaFunction( QskMetaFunction&& other )
    : m_functionCall( other.m_functionCall )
{
    other.m_functionCall = nullptr;
}

QskMetaFunction::~QskMetaFunction()
{
    if ( m_functionCall )
        m_functionCall->destroyIfLastRef();
}

QskMetaFunction& QskMetaFunction::operator=( QskMetaFunction&& other )
{
    if ( m_functionCall != other.m_functionCall )
    {
        if ( m_functionCall )
            m_functionCall->destroyIfLastRef();

        m_functionCall = other.m_functionCall;
        other.m_functionCall = nullptr;
    }

    return *this;
}

QskMetaFunction& QskMetaFunction::operator=( const QskMetaFunction& other )
{
    if ( m_functionCall != other.m_functionCall )
    {
        if ( m_functionCall )
            m_functionCall->destroyIfLastRef();

        m_functionCall = other.m_functionCall;

        if ( m_functionCall )
            m_functionCall->ref();
    }

    return *this;
}

bool QskMetaFunction::operator==( const QskMetaFunction& other ) const
{
    if ( m_functionCall == other.m_functionCall )
        return true;

    /*
        There is no way to compmare functors/members without
        std::type_info, what we don't want to use as it is
        another template creating symbols.

        So this implementation can't do much more than finding
        out if one instance is a copy from another.
     */

    if ( m_functionCall && other.m_functionCall )
    {
        if ( m_functionCall->typeInfo() == StaticFunction &&
             other.m_functionCall->typeInfo() == StaticFunction )
        {
            // only static functions can be compared
            return m_functionCall->compare(
                reinterpret_cast< void** >( other.m_functionCall ) );
        }
    }

    return false;
}

int QskMetaFunction::returnType() const
{
    return QMetaType::Void; // TODO ...
}

size_t QskMetaFunction::parameterCount() const
{
    int count = 0;

    if ( auto types = parameterTypes() )
    {
        while ( types[ count ] != QMetaType::UnknownType )
            count++;
    }

    return count;
}

QskMetaFunction::Type QskMetaFunction::functionType() const
{
    if ( m_functionCall == nullptr )
        return Invalid;

    return static_cast< QskMetaFunction::Type >( m_functionCall->typeInfo() );
}

void QskMetaFunction::invoke( QObject* object,
    void* argv[], Qt::ConnectionType connectionType )
{
#if 1
    /*
        Since Qt 5.10 we also have QMetaObject::invokeMethod
        with functor based callbacks. TODO ...
     */
#endif

    // code is not thread safe - TODO ...

    QPointer< QObject > receiver( object );

    if ( m_functionCall == nullptr )
        return;

    int invokeType = connectionType & 0x3;

    if ( invokeType == Qt::AutoConnection )
    {
        invokeType = ( receiver && receiver->thread() != QThread::currentThread() )
            ? Qt::QueuedConnection : Qt::DirectConnection;
    }

    switch ( invokeType )
    {
        case Qt::DirectConnection:
        {
            m_functionCall->call( receiver, argv );
            break;
        }
        case Qt::BlockingQueuedConnection:
        {
            if ( receiver.isNull() ||
                ( receiver->thread() == QThread::currentThread() ) )
            {
                // We would end up in a deadlock, better do nothing
                return;
            }

            QSemaphore semaphore;

            qskInvokeFunctionQueued( receiver,
                m_functionCall, 0, nullptr, argv, &semaphore );

            semaphore.acquire();

            break;
        }
        case Qt::QueuedConnection:
        {
            if ( receiver.isNull() )
            {
                return;
            }

            const auto argc = parameterCount() + 1; // return value + arguments

            auto types = static_cast< int* >( malloc( argc * sizeof( int ) ) );
            auto arguments = static_cast< void** >( malloc( argc * sizeof( void* ) ) );

            types[ 0 ] = QMetaType::UnknownType;
            arguments[ 0 ] = nullptr;

            const int* parameterTypes = m_functionCall->parameterTypes();
            for ( uint i = 1; i < argc; i++ )
            {
                if ( argv[ i ] == nullptr )
                {
                    Q_ASSERT( arguments[ i ] != nullptr );
                    receiver = nullptr;
                    break;
                }

                types[ i ] = parameterTypes[ i - 1 ];
                arguments[ i ] = QMetaType::create( types[ i ], argv[ i ] );
            }

            if ( receiver.isNull() )
            {
                // object might have died in the meantime
                free( types );
                free( arguments );

                return;
            }

            qskInvokeFunctionQueued( object, m_functionCall, argc, types, arguments );
            break;
        }
    }
}

#include "moc_QskMetaFunction.cpp"
