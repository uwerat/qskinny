/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskMetaCallback.h"
#include "QskMetaMethod.h"

QskMetaCallback::QskMetaCallback( const QObject* object,
        const QMetaMethod& method, Qt::ConnectionType connectionType ):
    m_object( const_cast< QObject* >( object ) ),
    m_invokable( method ),
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
        const QMetaProperty& property, Qt::ConnectionType connectionType ):
    m_object( const_cast< QObject* >( object ) ),
    m_invokable( property ),
    m_hasObject( object != nullptr ),
    m_connectionType( static_cast< ushort >( connectionType & 0x3 ) )
{
}

QskMetaCallback::QskMetaCallback( const QObject* object,
        const QskMetaFunction& function, Qt::ConnectionType connectionType ):
    m_object( const_cast< QObject* >( object ) ),
    m_invokable( function ),
    m_hasObject( object != nullptr ),
    m_connectionType( static_cast< ushort >( connectionType & ~Qt::UniqueConnection ) )
{
}

QskMetaCallback::~QskMetaCallback()
{
}

bool QskMetaCallback::isValid() const
{
    if ( m_hasObject && m_object.isNull() )
        return false;

    return !m_invokable.isNull();
}

void QskMetaCallback::setInvokable( const QskMetaInvokable& invokable )
{
    // type checking is m_object matches ???
    m_invokable = invokable;
}

void QskMetaCallback::setConnectionType( Qt::ConnectionType connectionType )
{
    m_connectionType = connectionType;
}

void QskMetaCallback::invoke( void* args[] )
{
    if ( isValid() )
        m_invokable.invoke( m_object, args, connectionType() );
}
