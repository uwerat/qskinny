/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskMetaCallback.h"
#include "QskMetaFunction.h"
#include "QskMetaMethod.h"

#include <QMetaMethod>

namespace
{
    class Function: public QskMetaFunction
    {
    public:
        inline Function( void* functionCall ):
            QskMetaFunction( static_cast< FunctionCall* >( functionCall ) )
        {
        }

        static inline void ref( void* functionCall )
        {
            if ( functionCall )
                static_cast< FunctionCall* >( functionCall )->ref();
        }

        static inline void deref( void* functionCall )
        {
            if ( functionCall )
                static_cast< FunctionCall* >( functionCall )->destroyIfLastRef();
        }
    };
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
        const char* methodName, Qt::ConnectionType connectionType ):
    QskMetaCallback( object, QskMetaMethod::method( object, methodName ), connectionType )
{
}

QskMetaCallback::QskMetaCallback( const QObject* object,
        const QskMetaFunction& function, Qt::ConnectionType connectionType ):
    m_object( const_cast< QObject* >( object ) ),
    m_functionData { function.functionCall(), function.parameterTypes() },
    m_type( MetaFunction ),
    m_hasObject( object != nullptr ),
    m_connectionType( static_cast< ushort >( connectionType & ~Qt::UniqueConnection ) )
{
    Function::ref( m_functionData.functionCall );
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
            m_functionData.functionCall = other.m_functionData.functionCall;
            Function::ref( m_functionData.functionCall );

            m_functionData.parameterTypes = other.m_functionData.parameterTypes;
            break;
        }

        default:
            break;
    }
}

QskMetaCallback::~QskMetaCallback()
{
    if ( m_type == MetaFunction )
        Function::deref( m_functionData.functionCall );
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
            if ( m_type == MetaFunction )
                Function::deref( m_functionData.functionCall );

            m_methodData.metaObject = other.m_methodData.metaObject;
            m_methodData.methodIndex = other.m_methodData.methodIndex;

            break;
        }
        case MetaFunction:
        {
            if ( m_type == MetaFunction  )
                Function::deref( m_functionData.functionCall );

            m_functionData.functionCall = other.m_functionData.functionCall;
            Function::ref( m_functionData.functionCall );

            m_functionData.parameterTypes = other.m_functionData.parameterTypes;
            break;
        }

        default:
            if ( m_type == MetaFunction )
                Function::deref( m_functionData.functionCall );
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
            return m_functionData.functionCall != nullptr;
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

    if ( m_type == MetaFunction )
        Function::deref( m_functionData.functionCall );

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
            QskMetaMethod::invoke( object, m_methodData.metaObject,
                m_methodData.methodIndex, args, connectionType() );

            break;
        }
        case MetaFunction:
        {
            if ( m_functionData.functionCall )
            {
                Function function( m_functionData.functionCall );
                function.invoke( object, args, connectionType() );
            }

            break;
        }

        default:
            break;
    }
}
