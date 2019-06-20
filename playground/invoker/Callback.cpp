/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Callback.h"
#include <QMetaMethod>

Callback::Callback()
    : m_context( nullptr )
{
}

Callback::Callback( const QObject* context, const QMetaMethod& method )
    : m_context( const_cast< QObject* >( context ) )
    , m_invokable( method )
{
}

Callback::Callback( const QObject* context, const char* methodName )
    : Callback( context, qskMetaMethod( context, methodName ) )
{
}

Callback::Callback( const QObject* context, const QMetaProperty& property )
    : m_context( const_cast< QObject* >( context ) )
    , m_invokable( property )
{
}

Callback::Callback( const QObject* context, const QskMetaFunction& function )
    : m_context( const_cast< QObject* >( context ) )
    , m_invokable( function )
{
}

void Callback::invoke( void* args[], Qt::ConnectionType connectionType )
{
    m_invokable.invoke( m_context, args, connectionType );
}
