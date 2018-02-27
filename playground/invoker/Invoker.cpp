/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Invoker.h"

Invoker::Invoker( QObject* parent ):
    QObject( parent )
{
}

void Invoker::addCallback( const QObject* object,
    const QskMetaFunction& function )
{
    m_callbacks += QskMetaCallback( object, function );
}

void Invoker::invoke( qreal realValue, int intValue,
    Qt::ConnectionType connectionType )
{
    for ( auto& callback : m_callbacks )
    {
        void* args[3] = { nullptr };

        const auto types = callback.parameterTypes();

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
                default:
                    break;
            }
        }

        callback.setConnectionType( connectionType );

        if ( connectionType == Qt::DirectConnection )
            callback.invoke( args );
        else if ( callback.object() )
            callback.invoke( args );
    }
}
