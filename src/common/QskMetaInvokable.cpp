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

static void qskInvokeSetProperty( QObject* object,
    const QMetaObject* metaObject, int propertyIndex,
    void* args[], Qt::ConnectionType connectionType )
{
#if 1
    if ( connectionType != Qt::DirectConnection )
        return; // TODO ...
#endif

    if ( propertyIndex >= 0 && object->metaObject() == metaObject )
    {
        int status = -1;
        int flags = 0;
        void *argv[] = { args[1], nullptr, &status, &flags };

#if 1
        QMetaObject::metacall( object,
            QMetaObject::WriteProperty, propertyIndex, argv );
#else
        metaObject->d.static_metacall(object, QMetaObject::WriteProperty, idx, argv);
#endif
    }
}

QskMetaInvokable::QskMetaInvokable( const QMetaMethod& method ):
    m_metaData { method.enclosingMetaObject(), method.methodIndex()  },
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

QskMetaInvokable::QskMetaInvokable( const QMetaProperty& property ):
    m_metaData { property.enclosingMetaObject(), property.propertyIndex() },
    m_type( MetaProperty )
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
        case MetaProperty:
        {
            m_metaData.metaObject = other.m_metaData.metaObject;
            m_metaData.index = other.m_metaData.index;

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
        case MetaProperty:
        {
            if ( m_type == MetaFunction )
                Function::deref( m_functionData.functionCall );

            m_metaData.metaObject = other.m_metaData.metaObject;
            m_metaData.index = other.m_metaData.index;

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
    if ( m_type != other.m_type )
        return false;

    switch( m_type )
    {
        case MetaMethod:
        case MetaProperty:
        {
            return ( m_metaData.metaObject == other.m_metaData.metaObject )
                && ( m_metaData.index == other.m_metaData.index );
        }
        case MetaFunction:
        {
            return m_functionData.functionCall == other.m_functionData.functionCall;
        }
        default:
        {
            return true;
        }
    }
}

bool QskMetaInvokable::isNull() const
{
    switch( m_type )
    {
        case MetaMethod:
        case MetaProperty:
        {
            const auto& d = m_metaData;

            if ( d.metaObject == nullptr || d.index < 0 )
                return true;

            const int count = ( m_type == MetaMethod )
                ? d.metaObject->methodCount() : d.metaObject->propertyCount();

            return d.index >= count;
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
            // should be doable without QMetaMethod. TODO ...
            const auto method = QskMetaInvokable::method();

            const int paramCount = method.parameterCount();
            if ( paramCount > 0 )
            {
                paramTypes.reserve( paramCount );

                for ( int i = 0; i < paramCount; i++ )
                    paramTypes += method.parameterType( i );
            }

            break;
        }
        case MetaProperty:
        {
            // should be doable without QMetaProperty. TODO ...
            const auto property = QskMetaInvokable::property();
            if ( property.isWritable() )
            {
                paramTypes.reserve( 1 );
                paramTypes += property.userType();
            }

            break;
        }
        case MetaFunction:
        {
            auto types = function().parameterTypes();
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

int QskMetaInvokable::returnType() const
{
    switch( m_type )
    {
        case MetaMethod:
        {
            return method().returnType();
        }
        case MetaFunction:
        {
            return function().returnType();
        }
        case MetaProperty:
        default:
        {
            return QMetaType::Void;
        }
    }
}

QMetaMethod QskMetaInvokable::method() const
{
    if ( m_type == MetaMethod && m_metaData.metaObject )
        return m_metaData.metaObject->method( m_metaData.index );

    return QMetaMethod();
}

QMetaProperty QskMetaInvokable::property() const
{
    if ( m_type == MetaProperty && m_metaData.metaObject )
        return m_metaData.metaObject->property( m_metaData.index );

    return QMetaProperty();
}

QskMetaFunction QskMetaInvokable::function() const
{
    if ( m_type == MetaFunction && m_functionData.functionCall )
    {
        Function function( m_functionData.functionCall );
        return *static_cast< QskMetaFunction* >( &function );
    }

    return QskMetaFunction();
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
            QskMetaMethod::invoke( object, m_metaData.metaObject,
                m_metaData.index, args, connectionType );

            break;
        }
        case MetaProperty:
        {
            qskInvokeSetProperty( object,
                m_metaData.metaObject, m_metaData.index,
                args, connectionType );

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
