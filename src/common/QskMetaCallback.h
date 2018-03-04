/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_META_CALLBACK_H
#define QSK_META_CALLBACK_H 1

#include "QskGlobal.h"
#include "QskMetaInvokable.h"

#include <QPointer>
#include <QObject>

class QSK_EXPORT QskMetaCallback
{
public:
    QskMetaCallback();

    QskMetaCallback( const QObject*, const QskMetaFunction&,
        Qt::ConnectionType = Qt::AutoConnection );

    QskMetaCallback( const QObject*, const QMetaMethod&,
        Qt::ConnectionType = Qt::AutoConnection );

    QskMetaCallback( const QObject*, const char* methodName,
        Qt::ConnectionType = Qt::AutoConnection );

    ~QskMetaCallback();

    bool isValid() const;
    bool isStale() const;

    const QObject* object() const { return m_object; }

    void setInvokable( const QskMetaInvokable& );
    const QskMetaInvokable& invokable() const;

    void setConnectionType( Qt::ConnectionType );
    Qt::ConnectionType connectionType() const;

    void invoke( void* args[] );

private:
    QPointer< QObject > m_object;
    QskMetaInvokable m_invokable;

    bool m_hasObject : 1;
    ushort m_connectionType : 3;
};

inline QskMetaCallback::QskMetaCallback():
    m_hasObject( false )
{
}

inline bool QskMetaCallback::isStale() const
{
    return m_hasObject && m_object.isNull();
}

inline const QskMetaInvokable& QskMetaCallback::invokable() const
{
    return m_invokable;
}

inline Qt::ConnectionType QskMetaCallback::connectionType() const
{
    return static_cast< Qt::ConnectionType >( m_connectionType );
}

Q_DECLARE_METATYPE( QskMetaCallback )

#endif
