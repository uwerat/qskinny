/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_META_CALLBACK_H
#define QSK_META_CALLBACK_H 1

#include "QskGlobal.h"
#include "QskMetaFunction.h"

#include <QObject>
#include <QPointer>
#include <QMetaMethod>
#include <QVector>

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

    union
    {
        QskMetaFunction* m_function;
        QMetaMethod* m_method;
    };

    int m_type : 3;
    ushort m_connectionType : 3;
};

inline QskMetaCallback::QskMetaCallback():
    m_type( Invalid )
{
}

inline bool QskMetaCallback::isValid() const
{
    return m_type > 0;
}

inline QskMetaCallback::Type QskMetaCallback::type() const
{
    return static_cast< Type >( m_type );
}

inline Qt::ConnectionType QskMetaCallback::connectionType() const
{
    return static_cast< Qt::ConnectionType >( m_connectionType );
}

Q_DECLARE_METATYPE( QskMetaCallback )

#endif
