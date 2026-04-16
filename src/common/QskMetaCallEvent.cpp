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
    class MetaCallEvent final : public QMetaCallEvent
    {
      public:
        MetaCallEvent( QMetaObject::Call call, const QMetaObject* metaObject,
                ushort offset, ushort index, void* args[], QskMetaCallLatch* latch )
            : QMetaCallEvent( offset, index,
                metaObject->d.static_metacall, nullptr, -1, args, latch )
            , m_call( call )
            , m_callFunction( metaObject->d.static_metacall )
            , m_index( index )
        {
        }

        void placeMetaCall( QObject* object ) override
        {
            m_callFunction( object, m_call, m_index, args() );
        }

      private:
#if QT_VERSION >= QT_VERSION_CHECK( 6, 11, 0 )
        inline void** args() { return d.args_; }
#endif

        const QMetaObject::Call m_call;

        QObjectPrivate::StaticMetaCallFunction m_callFunction;
        const ushort m_index;
    };

#if QT_VERSION >= QT_VERSION_CHECK( 6, 11, 0 )
    using MyMetaCallEvent = QQueuedMetaCallEvent;
#else
    using MyMetaCallEvent = QMetaCallEvent;
#endif

    class QueuedMetaCallEvent final : public MyMetaCallEvent
    {
      public:
#if QT_VERSION >= QT_VERSION_CHECK( 6, 11, 0 )
        QueuedMetaCallEvent( QMetaObject::Call call, const QMetaObject* metaObject,
                ushort offset, ushort index, int argc,
                const QtPrivate::QMetaTypeInterface* const* ifaces, void* args[] )
            : MyMetaCallEvent( offset, index, metaObject->d.static_metacall,
                nullptr, -1, argc, ifaces, args )
            , m_call( call )
            , m_callFunction( metaObject->d.static_metacall )
            , m_index( index )
        {
        }
#else
        QueuedMetaCallEvent( QMetaObject::Call call, const QMetaObject* metaObject,
                ushort offset, ushort index, int argc )
            : MyMetaCallEvent( offset, index, metaObject->d.static_metacall,
                nullptr, -1, argc )
            , m_call( call )
            , m_callFunction( metaObject->d.static_metacall )
            , m_index( index )
        {
        }
#endif

        void placeMetaCall( QObject* object ) override
        {
#if QT_VERSION >= QT_VERSION_CHECK( 6, 11, 0 )
            auto args = d.args_;
            if ( m_call == QMetaObject::WriteProperty )
                args++;

            m_callFunction( object, m_call, m_index, args );
#else
            auto args = this->args();
#endif

            m_callFunction( object, m_call, m_index, args );
        }

      private:
        const QMetaObject::Call m_call;

        QObjectPrivate::StaticMetaCallFunction m_callFunction;
        const ushort m_index;
    };
}

QMetaCallEvent* qskCreateQueuedMethodCallEvent(
    const QMetaObject* metaObject, ushort offset, ushort index, void* args[] )
{
    const auto method = metaObject->method( offset + index );
    const int argc = method.parameterCount() + 1;

#if QT_VERSION >= QT_VERSION_CHECK( 6, 11, 0 )
    QVarLengthArray< const QtPrivate::QMetaTypeInterface* > ifaces( argc );

    ifaces[0] = nullptr;
    for ( int i = 1; i < argc; i++ )
    {
        const QMetaType metaType( method.parameterType( i - 1 ) );
        ifaces[i] = metaType.iface();
    }

    auto event = new QueuedMetaCallEvent( QMetaObject::InvokeMetaMethod,
        metaObject, offset, index, argc, ifaces.constData(), args );
#else
    auto event = new QueuedMetaCallEvent(
        QMetaObject::InvokeMetaMethod, metaObject, offset, index, argc );

    /*
        The first one is the return type, one that is always
        invalid for Queued Connections.
     */

    auto types = event->types();
    auto arguments = event->args();

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
    types[0] = QMetaType();
    arguments[ 0 ] = nullptr;
#else
    types[0] = 0;
    arguments[ 0 ] = nullptr;
#endif

    for ( int i = 1; i < argc; i++ )
    {
        const auto type = method.parameterType( i - 1 );
        const auto arg = args[ i ];

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
        types[ i ] = QMetaType( type );
        arguments[ i ] = QMetaType( type ).create( arg );
#else
        types[ i ] = type;
        arguments[ i ] = QMetaType::create( type, arg );
#endif
    }

#endif

    return event;
}

QMetaCallEvent* qskCreateQueuedWritePropertyCallEvent(
    const QMetaObject* metaObject, ushort offset, ushort index, void* args[] )
{
    const auto property = metaObject->property( offset + index );

#if QT_VERSION >= QT_VERSION_CHECK( 6, 11, 0 )
    const QtPrivate::QMetaTypeInterface* ifaces[2];
    ifaces[0] = nullptr;
    ifaces[1] = QMetaType( property.userType() ).iface();

    void* argv[2] = { nullptr, args[0] };

    auto event = new QueuedMetaCallEvent( QMetaObject::WriteProperty,
        metaObject, offset, index, 2, ifaces, argv );
#else
    auto event = new QueuedMetaCallEvent( QMetaObject::WriteProperty,
        metaObject, offset, index, 1 );

    const auto type = property.userType();
    const auto arg = args[ 0 ];

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
    event->types()[0] = QMetaType( type );
    event->args()[ 0 ] = QMetaType( type ).create( arg );
#else
    event->types()[0] = type;
    event->args()[ 0 ] = QMetaType::create( type, arg );
#endif

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
    auto event = new QMetaCallEvent( functionCall, nullptr, 0, argc );

    auto typesEv = event->types();
    auto argsEv = event->args();

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
    typesEv[0] = QMetaType();
    argsEv[ 0 ] = nullptr;
#else
    typesEv[0] = 0;
    argsEv[ 0 ] = nullptr;
#endif

    for ( int i = 1; i < argc; i++ )
    {
        const auto type = parameterTypes[i - 1];

#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
        typesEv[ i ] = QMetaType( type );
        argsEv[ i ] = QMetaType( type ).create( args[ i ] );
#else
        typesEv[ i ] = type;
        argsEv[ i ] = QMetaType::create( type, args[ i ] );
#endif
    }
#endif

    return event;
}

QMetaCallEvent* qskCreateMetaObjectCallEvent(
    QMetaObject::Call call, const QMetaObject* metaObject,
    ushort offset, ushort index, void* args[], QskMetaCallLatch* latch )
{
    return new MetaCallEvent( call, metaObject, offset, index, args, latch );
}
