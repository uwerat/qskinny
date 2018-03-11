/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Invoker.h"

#include <QThread>
#include <QMetaProperty>

Invoker::Invoker( QObject* parent ):
    QObject( parent )
{
}

void Invoker::addFunctionCall( const QObject* object,
    const QskMetaFunction& function )
{
    m_callbacks.append( Callback( object, function ) );
}

void Invoker::addMethodCall( const QObject* object,
    const char* methodName )
{
    m_callbacks.append( Callback( object, methodName ) );
}

void Invoker::addPropertyCall( const QObject* object,
    const char* property )
{
    const auto metaProperty = object->metaObject()->property(
        object->metaObject()->indexOfProperty( property ) );

    m_callbacks.append( Callback( object, metaProperty ) );
}

void Invoker::invoke( qreal realValue, int intValue,
    Qt::ConnectionType connectionType )
{
    QString s = QString( "S: %1 + %2" ).arg( realValue ).arg( intValue );

    for ( auto& callback : m_callbacks )
    {
        void* args[4] = { nullptr };

        const auto types = callback.invokable().parameterTypes();

        int i = 1;
        for ( auto type : types )
        {
            switch ( type )
            {
                case QMetaType::Int:
                {
                    args[i++] = reinterpret_cast< void* >( &intValue );
                    break;
                }
                case QMetaType::Double:
                {
                    args[i++] = reinterpret_cast< void* >( &realValue );
                    break;
                }
                case QMetaType::QString:
                {
                    args[i++] = reinterpret_cast< void* >( &s );
                    break;
                }
                default:
                    break;
            }
        }

        switch( connectionType & 0x3 )
        {
            case Qt::DirectConnection:
            {
                callback.invoke( args, connectionType );
                break;
            }
            case Qt::QueuedConnection:
            {
                if ( callback.context() )
                    callback.invoke( args, connectionType );

                break;
            }
            case Qt::BlockingQueuedConnection:
            {
                const auto receiver = callback.context();
                if ( receiver && receiver->thread() != QThread::currentThread() )
                    callback.invoke( args, connectionType );

                break;
            }
        }
    }
}
