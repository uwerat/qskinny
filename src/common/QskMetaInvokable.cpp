/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskMetaInvokable.h"
#include "QskMetaFunction.h"
#include "QskMetaMethod.h"

#include <QMetaMethod>
#include <QVector>
#include <QObject>

namespace
{
    class Function : public QskMetaFunction
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


QskMetaInvokable::QskMetaInvokable( const QMetaMethod& method ):
    m_methodData { method.enclosingMetaObject(), method.methodIndex()  },
    m_type( MetaMethod )
{
}

QskMetaInvokable::QskMetaInvokable( const QObject* object, const char* methodName ):
    QskMetaInvokable( QskMetaMethod::method( object, methodName ) )
{
}

QskMetaInvokable::QskMetaInvokable( const QMetaObject* metaObject, const char* methodName ):
    QskMetaInvokable( QskMetaMethod::method( metaObject, methodName ) )
{
}

QskMetaInvokable::QskMetaInvokable( const QskMetaFunction& function ):
    m_functionData { function.functionCall() },
    m_type( MetaFunction )
{
    Function::ref( m_functionData.functionCall );
}

QskMetaInvokable::QskMetaInvokable( const QskMetaInvokable& other ):
    m_type( other.m_type )
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

            break;
        }

        default:
            break;
    }
}

QskMetaInvokable::~QskMetaInvokable()
{
    if ( m_type == MetaFunction )
        Function::deref( m_functionData.functionCall );
}

QskMetaInvokable& QskMetaInvokable::operator=( const QskMetaInvokable& other )
{
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

            break;
        }

        default:
            if ( m_type == MetaFunction )
                Function::deref( m_functionData.functionCall );
    }

    m_type = other.m_type;

    return *this;
}

bool QskMetaInvokable::operator==( const QskMetaInvokable& other ) const
{
    if ( m_type == other.m_type )
    {
        if ( m_type == MetaMethod )
        {
            return ( m_methodData.metaObject == other.m_methodData.metaObject )
                && ( m_methodData.methodIndex == other.m_methodData.methodIndex );
        }
        if ( m_type == MetaFunction )
        {
            return m_functionData.functionCall == other.m_functionData.functionCall;
        }
    }

    return true;
}

bool QskMetaInvokable::isNull() const
{
    switch( m_type )
    {
        case MetaMethod:
        {
            const auto& d = m_methodData;
            if ( d.metaObject && ( d.methodIndex >= 0 )
                && ( d.methodIndex < d.metaObject->methodCount() ) )
            {
                return false;
            }

            return true;
        }

        case MetaFunction:
        {
            return m_functionData.functionCall == nullptr;
        }

        default:
            return true;
    }
}

void QskMetaInvokable::reset()
{
    if ( m_type == MetaFunction )
        Function::deref( m_functionData.functionCall );

    m_type = Invalid;
}

QVector< int > QskMetaInvokable::parameterTypes() const
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
            auto types = Function( m_functionData.functionCall ).parameterTypes();
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


void QskMetaInvokable::invoke( QObject* object, void* args[],
    Qt::ConnectionType connectionType )
{
    if ( isNull() )
        return;

    switch( m_type )
    {
        case MetaMethod:
        {
            QskMetaMethod::invoke( object, m_methodData.metaObject,
                m_methodData.methodIndex, args, connectionType );

            break;
        }
        case MetaFunction:
        {
            if ( m_functionData.functionCall )
            {
                Function function( m_functionData.functionCall );
                function.invoke( object, args, connectionType );
            }

            break;
        }

        default:
            break;
    }
}
