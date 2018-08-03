/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef INVOKER_H
#define INVOKER_H 1

#include "Callback.h"
#include <QskMetaFunction.h>

#include <QObject>
#include <QVector>

class Invoker : public QObject
{
  public:
    Invoker( QObject* parent = nullptr );

    void addFunctionCall( const QskMetaFunction& );
    void addFunctionCall( const QObject*, const QskMetaFunction& );

    void addMethodCall( const QObject*, const char* methodName );
    void addPropertyCall( const QObject*, const char* property );

    void invoke( qreal d, int i, Qt::ConnectionType );

  private:
    QVector< Callback > m_callbacks;
};

inline void Invoker::addFunctionCall( const QskMetaFunction& function )
{
    addFunctionCall( this, function );
}

#endif
