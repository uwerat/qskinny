/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

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
