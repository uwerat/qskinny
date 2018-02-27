/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskMetaCallback.h"
#include <QObject>
#include <QVector>

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

void QskMetaCallback::setConnectionType( Qt::ConnectionType connectionType )
{
    m_connectionType = connectionType;
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
                QskMetaCall::invoke( object, m_method, args, connectionType() );
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
