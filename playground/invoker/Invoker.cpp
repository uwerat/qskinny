/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Invoker.h"
#include <QThread>

Invoker::Invoker( QObject* parent ):
    QObject( parent )
{
}

void Invoker::addCallback( const QObject* object,
    const QskMetaFunction& function )
{
    m_callbacks.append( QskMetaCallback( object, function ) );
}

void Invoker::addCallback( const QObject* object,
    const char* methodName )
{
    m_callbacks.append( QskMetaCallback( object, methodName ) );
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

        callback.setConnectionType( connectionType );

        const int callType = connectionType & 0x3;
        switch( callType )
        {
            case Qt::DirectConnection:
            {
                callback.invoke( args );
                break;
            }
            case Qt::QueuedConnection:
            {
                if ( callback.object() )
                    callback.invoke( args );
                break;
            }
            case Qt::BlockingQueuedConnection:
            {
                const auto receiver = callback.object();
                if ( receiver && receiver->thread() != QThread::currentThread() )
                    callback.invoke( args );

                break;
            }
        }
    }
}
