/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_META_CALLBACK_H
#define QSK_META_CALLBACK_H 1

#include "QskGlobal.h"

#include <QPointer>
#include <QVector>

class QskMetaInvokable;
class QskMetaFunction;
class QMetaMethod;

class QskMetaCallback
{
public:
    enum Type
    {
        Invalid = 0,

        // A QMetaMethod
        MetaMethod,

        // A function pointer, for what Qt calls "functor based" callbacks
        MetaFunction
    };

    QskMetaCallback();

    QskMetaCallback( const QObject*, const QskMetaFunction&,
        Qt::ConnectionType = Qt::AutoConnection );

    QskMetaCallback( const QObject*, const QMetaMethod&,
        Qt::ConnectionType = Qt::AutoConnection );

    QskMetaCallback( const QskMetaCallback& );

    ~QskMetaCallback();

    QskMetaCallback& operator=( const QskMetaCallback& );

    Type type() const;
    bool isValid() const;

    const QObject* object() const { return m_object; }

    void setConnectionType( Qt::ConnectionType );
    Qt::ConnectionType connectionType() const;

    QVector< int > parameterTypes() const;

    void invoke( void* args[] );

private:
    void reset();

    QPointer< const QObject > m_object;

    struct FunctionData
    {
        QskMetaInvokable* invokable;
        const int* parameterTypes;
    };

    struct MethodData
    {
        const QMetaObject* metaObject;
        int methodIndex;
    };

    union
    {
        FunctionData m_functionData;
        MethodData m_methodData;
    };

    int m_type : 3;
    bool m_hasObject : 1;
    ushort m_connectionType : 3;
};

inline QskMetaCallback::QskMetaCallback():
    m_type( Invalid ),
    m_hasObject( false )
{
}

inline QskMetaCallback::Type QskMetaCallback::type() const
{
    return static_cast< Type >( m_type );
}

inline Qt::ConnectionType QskMetaCallback::connectionType() const
{
    return static_cast< Qt::ConnectionType >( m_connectionType );
}

QSK_EXPORT void qskInvokeMethod(
    QObject* object, const QMetaMethod&, void* args[],
    Qt::ConnectionType = Qt::AutoConnection );

QSK_EXPORT void qskInvokeMethod(
    QObject* object, const QMetaObject*, int methodIndex, void* args[],
    Qt::ConnectionType = Qt::AutoConnection );

Q_DECLARE_METATYPE( QskMetaCallback )

#endif
