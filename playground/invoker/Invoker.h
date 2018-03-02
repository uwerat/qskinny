/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef INVOKER_H
#define INVOKER_H 1

#include <QskMetaFunction.h>
#include <QskMetaCallback.h>

#include <QObject>
#include <QVector>

class Invoker : public QObject
{
public:
    Invoker( QObject* parent = nullptr );

    void addCallback( const QskMetaFunction& );
    void addCallback( const QObject*, const QskMetaFunction& );
    void addCallback( const QObject*, const char* methodName );

    void invoke( qreal d, int i, Qt::ConnectionType );

private:
    QVector< QskMetaCallback > m_callbacks;
};

inline void Invoker::addCallback( const QskMetaFunction& function )
{
    addCallback( this, function );
}

#endif
