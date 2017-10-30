/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskShortcut.h"

#include <QObject>
#include <QKeySequence>
#include <QQuickWindow>
#include <QtGui/private/qguiapplication_p.h>
#include <QMap>

class QskShortcutHandler final : public QObject
{
public:
    static QskShortcutHandler* instance();

    int addShortcut( QQuickWindow*, const QKeySequence&,
        QObject* receiver, const QMetaMethod&, Qt::ConnectionType type );

    void setEnabled( int id, bool );
    void setAutoRepeat( int id, bool repeat );

protected:
    virtual bool event( QEvent* event ) override final;

private:
    QskShortcutHandler() = default;

    static bool contextMatcher( QObject*, Qt::ShortcutContext );
    static QShortcutMap& map();

    struct InvokeData
    {
        QObject* receiver;
        QMetaMethod method;
        Qt::ConnectionType connectionType;
    };

    QMap< int, InvokeData > m_invokeDataMap;
};

QskShortcutHandler* QskShortcutHandler::instance()
{
    static QskShortcutHandler handler;
    return &handler;
}

inline QShortcutMap& QskShortcutHandler::map()
{
    return QGuiApplicationPrivate::instance()->shortcutMap;
}

int QskShortcutHandler::addShortcut( QQuickWindow* window, const QKeySequence& key,
    QObject* receiver, const QMetaMethod& method, Qt::ConnectionType connectionType )
{
    int id;

    if ( window )
        id = map().addShortcut( window, key, Qt::WindowShortcut, contextMatcher );
    else
        id = map().addShortcut( this, key, Qt::ApplicationShortcut, contextMatcher );

    InvokeData& data = m_invokeDataMap[ id ];
    data.receiver = receiver;
    data.method = method;
    data.connectionType = connectionType;

    return id;
}

bool QskShortcutHandler::contextMatcher( QObject* obj, Qt::ShortcutContext context )
{
    switch ( context )
    {
        case Qt::ApplicationShortcut:
        {
            return true;
        }
        case Qt::WindowShortcut:
        {
            return obj == QGuiApplication::focusWindow();
        }
        default:
            return false;
    }
}

void QskShortcutHandler::setEnabled( int id, bool enabled )
{
    map().setShortcutEnabled( enabled, id, this );
}

void QskShortcutHandler::setAutoRepeat( int id, bool repeat )
{
    map().setShortcutAutoRepeat( repeat, id, this );
}

bool QskShortcutHandler::event( QEvent* event )
{
    if ( event->type() == QEvent::Shortcut )
    {
        const QShortcutEvent* se = static_cast< const QShortcutEvent* >( event );

        const auto it = m_invokeDataMap.constFind( se->shortcutId() );
        if ( it != m_invokeDataMap.constEnd() )
        {
            const InvokeData& data = ( *it );
            data.method.invoke( data.receiver, data.connectionType );
        }

        return true;
    }

    return false;
}

int QskShortcut::addShortcut( const QKeySequence& key,
    QObject* receiver, const char* method,
    bool autoRepeat, Qt::ConnectionType connectionType )
{
    return addShortcut( nullptr, key, receiver, method, autoRepeat, connectionType );
}

int QskShortcut::addShortcut( QQuickWindow* window, const QKeySequence& key,
    QObject* receiver, const char* method,
    bool autoRepeat, Qt::ConnectionType connectionType )
{
    int id = 0;

    if ( receiver == nullptr )
        return id;

    const QMetaObject* metaObject = receiver->metaObject();

    const int methodIndex = metaObject->indexOfMethod(
        QMetaObject::normalizedSignature( method ).constData() + 1 );

    if ( methodIndex >= 0 )
    {
        id = QskShortcutHandler::instance()->addShortcut( window, key,
                receiver, metaObject->method( methodIndex ), connectionType );

        if ( !autoRepeat )
            QskShortcutHandler::instance()->setAutoRepeat( id, false );
    }

    return id;
}

void QskShortcut::setAutoRepeat( int id, bool on )
{
    QskShortcutHandler::instance()->setAutoRepeat( id, on );
}
