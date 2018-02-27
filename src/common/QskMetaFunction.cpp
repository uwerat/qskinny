/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGlobal.h"
#include "QskMetaCall.h"
#include "QskMetaFunction.h"

QskMetaFunction::QskMetaFunction():
    m_invokable( nullptr ),
    m_parameterTypes( nullptr )
{
}

QskMetaFunction::QskMetaFunction( const QskMetaFunction& other ):
    m_invokable( other.m_invokable ),
    m_parameterTypes( other.m_parameterTypes )
{
    if ( m_invokable )
        m_invokable->ref();
}

QskMetaFunction::QskMetaFunction( QskMetaFunction&& other ):
    m_invokable( other.m_invokable ),
    m_parameterTypes( other.m_parameterTypes )
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

    m_parameterTypes = other.m_parameterTypes;

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

    m_parameterTypes = other.m_parameterTypes;

    return *this;
}

void QskMetaFunction::init( QskMetaCall::Invokable* invokable,
    const int* parameterTypes )
{
    m_invokable = invokable;
#if 0
    if ( m_invokable )
        m_invokable->ref();
#endif

    m_parameterTypes = parameterTypes;
}

const int* QskMetaFunction::parameterTypes() const
{
    return m_parameterTypes;
}

size_t QskMetaFunction::parameterCount() const
{
    if ( m_parameterTypes )
    {
        for ( int i = 1; ; i++ )
        {
            if ( m_parameterTypes[ i ] == QMetaType::UnknownType )
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
    if ( m_invokable )
    {
        QskMetaCall::invoke( object, *m_invokable,
            parameterCount(), parameterTypes(), argv, connectionType );
    }
}
