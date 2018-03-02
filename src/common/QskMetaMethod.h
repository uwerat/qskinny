/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_META_METHOD_H
#define QSK_META_FUNCTION_H 1

#include "QskGlobal.h"

#include <QObject>
#include <QMetaMethod>

class QObject;
class QMetaMethod;
class QMetaObject;

namespace QskMetaMethod
{
    QSK_EXPORT void invoke(
        QObject* object, const QMetaMethod&, void* args[],
        Qt::ConnectionType = Qt::AutoConnection );

    QSK_EXPORT void invoke(
        QObject*, const QMetaObject*, int methodIndex, void* args[],
        Qt::ConnectionType = Qt::AutoConnection );

    QSK_EXPORT QMetaMethod method( const QMetaObject*, const char* methodName );
    QSK_EXPORT QMetaMethod method( const QObject*, const char* methodName );

    QSK_EXPORT QMetaMethod notifySignal( const QMetaObject*, const char* propertyName );
    QSK_EXPORT QMetaMethod notifySignal( const QObject*, const char* propertyName );
}

inline QMetaMethod QskMetaMethod::method(
    const QObject* object, const char* methodName )
{
    return object ? method( object->metaObject(), methodName ) : QMetaMethod();
}

inline QMetaMethod QskMetaMethod::notifySignal(
    const QObject* object, const char* propertyName )
{
    return object ? notifySignal( object->metaObject(), propertyName ) : QMetaMethod();
}

#endif
