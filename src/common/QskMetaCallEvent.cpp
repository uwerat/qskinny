/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskMetaCallEvent.h"
#include "QskInternalMacros.h"
#include "QskMetaFunction.h"
#include "QskMetaFunction.hpp"

#include <qmetaobject.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qobject_p.h>
QSK_QT_PRIVATE_END

namespace
{
    using CallFunction = QObjectPrivate::StaticMetaCallFunction;

#if QT_VERSION >= QT_VERSION_CHECK( 6, 11, 0 )

    using TypeInterface = QtPrivate::QMetaTypeInterface;

    class UnqueuedMetaCallEvent final : public QMetaCallEvent
    {
      public:
        UnqueuedMetaCallEvent( QMetaObject::Call call, CallFunction callFunction,
                ushort offset, ushort index, void* args[], QLatch* latch )
            : QMetaCallEvent( offset, index, callFunction, nullptr, -1, args, latch )
            , m_call( call )
        {
        }

        void placeMetaCall( QObject* object ) override
        {
            d.callFunction_( object, m_call, d.method_relative_, d.args_ );
        }

      private:
        const QMetaObject::Call m_call;
    };

    class QueuedMetaCallEvent final : public QQueuedMetaCallEvent
    {
      public:
        QueuedMetaCallEvent( QMetaObject::Call call, CallFunction callFunction,
                ushort offset, ushort index, int argc,
                const TypeInterface* const * ifaces, void* args[] )
            : QQueuedMetaCallEvent( offset, index, callFunction,
                nullptr, -1, argc, ifaces, args )
            , m_call( call )
        {
        }

        void placeMetaCall( QObject* object ) override
        {
            auto args = d.args_;
            if ( m_call == QMetaObject::WriteProperty )
            {
                /*
                    Qt uses QQueuedMetaCallEvent for QMetaObject::InvokeMetaMethod,
                    where you always have the return value as first parameter.

                    However we also support QMetaObject::WriteProperty, where the
                    first parameter filled in by the Ctor is wrong.
                 */
                args++;
            }

            d.callFunction_( object, m_call, d.method_relative_, args );
        }

      private:
        const QMetaObject::Call m_call;
    };

#else
    class MetaCallEvent : public QMetaCallEvent
    {
      public:
        MetaCallEvent( QMetaObject::Call call, CallFunction callFunction,
                ushort offset, ushort index, void* args[], QSemaphore* semaphore )
            : QMetaCallEvent( offset, index, callFunction, nullptr, -1, args, semaphore )
            , m_call( call )
            , m_callFunction( callFunction )
            , m_index( index )
        {
        }

        MetaCallEvent( QMetaObject::Call call, CallFunction callFunction,
                ushort offset, ushort index, int argc )
            : QMetaCallEvent( offset, index, callFunction, nullptr, -1, argc )
            , m_call( call )
            , m_callFunction( callFunction )
            , m_index( index )
        {
        }

        MetaCallEvent( QtPrivate::QSlotObjectBase* slotObj, int argc )
            : QMetaCallEvent( slotObj, nullptr, -1, argc )
        {
        }

        inline void setArgument( int index, int type, void* arg )
        {
    #if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
            types()[ index ] = QMetaType( type );
            args()[ index ] = QMetaType( type ).create( arg );
    #else
            types()[ index ] = type;
            args()[ index ] = QMetaType::create( type, arg );
    #endif
        }

        void placeMetaCall( QObject* object ) override final
        {
            if ( m_callFunction )
            {
                m_callFunction( object, m_call, m_index, args() );
                return;
            }
            QMetaCallEvent::placeMetaCall( object );
        }

      private:
        const QMetaObject::Call m_call = QMetaObject::InvokeMetaMethod;

        CallFunction m_callFunction;
        const ushort m_index = -1;
    };

#endif
}

QMetaCallEvent* qskCreateQueuedMethodCallEvent(
    const QMetaObject* metaObject, ushort offset, ushort index, void* args[] )
{
    const auto method = metaObject->method( offset + index );
    const int argc = method.parameterCount() + 1;

    auto callFunction = metaObject->d.static_metacall;

#if QT_VERSION >= QT_VERSION_CHECK( 6, 11, 0 )
    QVarLengthArray< const QtPrivate::QMetaTypeInterface* > ifaces( argc );

    ifaces[0] = nullptr;
    for ( int i = 1; i < argc; i++ )
    {
        const QMetaType metaType( method.parameterType( i - 1 ) );
        ifaces[i] = metaType.iface();
    }

    auto event = new QueuedMetaCallEvent( QMetaObject::InvokeMetaMethod,
        callFunction, offset, index, argc, ifaces.constData(), args );
#else
    auto event = new MetaCallEvent( QMetaObject::InvokeMetaMethod,
        callFunction, offset, index, argc );

    /*
        The first one is the return type, one that is always
        invalid for Queued Connections.
     */

    event->setArgument( 0, QMetaType::UnknownType, nullptr );
    for ( int i = 1; i < argc; i++ )
        event->setArgument( i, method.parameterType( i - 1 ), args[ i ] );

#endif

    return event;
}

QMetaCallEvent* qskCreateQueuedWritePropertyCallEvent(
    const QMetaObject* metaObject, ushort offset, ushort index, void* args[] )
{
    const auto property = metaObject->property( offset + index );
    auto callFunction = metaObject->d.static_metacall;

#if QT_VERSION >= QT_VERSION_CHECK( 6, 11, 0 )

    // The first argument is a dummy return value
    const QtPrivate::QMetaTypeInterface* ifaces[2] =
        { nullptr, QMetaType( property.userType() ).iface() };

    void* argv[2] = { nullptr, args[0] };

    auto event = new QueuedMetaCallEvent( QMetaObject::WriteProperty,
        callFunction, offset, index, 2, ifaces, argv );
#else
    auto event = new MetaCallEvent( QMetaObject::WriteProperty,
        callFunction, offset, index, 1 );

    event->setArgument( 0, property.userType(), args[ 0 ] );
#endif

    return event;
}

QMetaCallEvent* qskCreateQueuedFunctorCallEvent(
    void* callBack, int argc, void* args[] )
{
    auto functionCall = reinterpret_cast< QskMetaFunction::FunctionCall* >( callBack );
    const int* parameterTypes = functionCall->parameterTypes();

#if QT_VERSION >= QT_VERSION_CHECK( 6, 11, 0 )
    QVarLengthArray< const QtPrivate::QMetaTypeInterface* > ifaces( argc );
    ifaces[0] = nullptr;

    for ( int i = 1; i < argc; i++ )
    {
        const QMetaType metaType( parameterTypes[i - 1] );
        ifaces[i] = metaType.iface();
    }

    auto event = new QQueuedMetaCallEvent( functionCall, nullptr, -1,
        argc, ifaces.constData(), args );
#else
    auto event = new MetaCallEvent( functionCall, argc );

    event->setArgument( 0, QMetaType::UnknownType, nullptr );
    for ( int i = 1; i < argc; i++ )
        event->setArgument( i, parameterTypes[i - 1], args[ i ] );
#endif

    return event;
}

QMetaCallEvent* qskCreateMetaObjectCallEvent(
    QMetaObject::Call call, const QMetaObject* metaObject,
    ushort offset, ushort index, void* args[], QskMetaCallLatch* latch )
{
    auto callFunction = metaObject->d.static_metacall;

#if QT_VERSION >= QT_VERSION_CHECK( 6, 11, 0 )
    return new UnqueuedMetaCallEvent( call, callFunction, offset, index, args, latch );
#else
    return new MetaCallEvent( call, callFunction, offset, index, args, latch );
#endif
}
