/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef CALLBACK_H
#define CALLBACK_H 1

#include <QskMetaInvokable.h>

class QObject;

class Callback
{
  public:
    Callback();

    Callback( const QObject*, const QskMetaFunction& );
    Callback( const QObject*, const QMetaMethod& );
    Callback( const QObject*, const QMetaProperty& );

    Callback( const QObject*, const char* methodName );

    void invoke( void* args[], Qt::ConnectionType );

    const QObject* context() const;
    const QskMetaInvokable& invokable() const;

  private:
    QObject* m_context;
    QskMetaInvokable m_invokable;
};

inline const QObject* Callback::context() const
{
    return m_context;
}

inline const QskMetaInvokable& Callback::invokable() const
{
    return m_invokable;
}

#endif
