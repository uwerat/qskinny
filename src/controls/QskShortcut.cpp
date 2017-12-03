/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskShortcut.h"

#include <QObject>
#include <QKeySequence>
#include <QQuickWindow>
#include <QMap>
#include <QGlobalStatic>
#include <QtGui/private/qguiapplication_p.h>

class QskShortcutHandler final : public QObject
{
public:
    int addShortcut( QQuickWindow*, const QKeySequence&,
        const QObject* receiver, const QMetaMethod& );

    int addShortcut( QQuickWindow*, const QKeySequence&,
        const QObject* receiver, QtPrivate::QSlotObjectBase* );

    void setEnabled( int id, bool );
    void setAutoRepeat( int id, bool repeat );

protected:
    virtual bool event( QEvent* event ) override final;

private:
    void cleanUp( QObject* );

    static bool contextMatcher( QObject*, Qt::ShortcutContext );
    static QShortcutMap& map();

    class InvokeData
    {
    public:
        ~InvokeData()
        {
            if ( slotObj )
                slotObj->destroyIfLastRef();
        }

        QMetaMethod method;
        QtPrivate::QSlotObjectBase* slotObj;
        const QObject* receiver;
    };

    QMap< int, InvokeData > m_invokeDataMap;
};

inline QShortcutMap& QskShortcutHandler::map()
{
    return QGuiApplicationPrivate::instance()->shortcutMap;
}

int QskShortcutHandler::addShortcut(
    QQuickWindow* window, const QKeySequence& key,
    const QObject* receiver, const QMetaMethod& method )
{
    int id;

    if ( window )
        id = map().addShortcut( window, key, Qt::WindowShortcut, contextMatcher );
    else
        id = map().addShortcut( this, key, Qt::ApplicationShortcut, contextMatcher );

    InvokeData& data = m_invokeDataMap[ id ];
    data.receiver = receiver;
    data.method = method;
    data.slotObj = nullptr;

    return id;
}

int QskShortcutHandler::addShortcut(
    QQuickWindow* window, const QKeySequence& key,
    const QObject* receiver, QtPrivate::QSlotObjectBase* slotObj )
{
    int id;

    if ( window )
        id = map().addShortcut( window, key, Qt::WindowShortcut, contextMatcher );
    else
        id = map().addShortcut( this, key, Qt::ApplicationShortcut, contextMatcher );

    InvokeData& data = m_invokeDataMap[ id ];
    data.receiver = receiver;
    data.slotObj = slotObj;

    if ( receiver )
        connect( receiver, &QObject::destroyed, this, &QskShortcutHandler::cleanUp );

    return id;
}

void QskShortcutHandler::cleanUp( QObject* receiver )
{
    map().removeShortcut( 0, receiver );

    for ( auto it = m_invokeDataMap.begin(); it != m_invokeDataMap.end(); )
    {
        if ( it->receiver == receiver )
        {
            it = m_invokeDataMap.erase( it );
            continue;
        }

        ++it;
    }
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
            auto receiver = const_cast< QObject* >( data.receiver );

            if ( data.slotObj )
            {
                void* args[] = { 0 };
                data.slotObj->call( receiver, args );
            }
            else
            {
                data.method.invoke( receiver, Qt::AutoConnection );
            }
        }

        return true;
    }

    return false;
}

Q_GLOBAL_STATIC( QskShortcutHandler, qskShortcutHandler )

int QskShortcut::addShortcut( const QKeySequence& key,
    bool autoRepeat, const QObject* receiver, const char* method )
{
    return addShortcut( nullptr, key, autoRepeat, receiver, method );
}

int QskShortcut::addShortcut( QQuickWindow* window, const QKeySequence& key,
    bool autoRepeat, const QObject* receiver, const char* method )
{
    int id = 0;

    if ( receiver == nullptr )
        return id;

    const QMetaObject* metaObject = receiver->metaObject();

    const int methodIndex = metaObject->indexOfMethod(
        QMetaObject::normalizedSignature( method ).constData() + 1 );

    if ( methodIndex >= 0 )
    {
        id = qskShortcutHandler->addShortcut(
            window, key, receiver, metaObject->method( methodIndex ) );

        if ( !autoRepeat )
            qskShortcutHandler->setAutoRepeat( id, false );
    }

    return id;
}

int QskShortcut::addShortcutImpl( const QKeySequence& key,
    bool autoRepeat, const QObject* receiver, QtPrivate::QSlotObjectBase* slotObj )
{
#if 1
    if ( receiver )
    {
        // how to call the slot in the receiver context, TODO ...
        Q_ASSERT( qskShortcutHandler->thread() == receiver->thread() );
    }
#endif

    QQuickWindow* window = nullptr;
    int id = qskShortcutHandler->addShortcut( window, key, receiver, slotObj );

    if ( !autoRepeat )
        qskShortcutHandler->setAutoRepeat( id, false );

    return id;
}

void QskShortcut::setAutoRepeat( int id, bool on )
{
    qskShortcutHandler->setAutoRepeat( id, on );
}

void QskShortcut::setEnabled( int id, bool on )
{
    qskShortcutHandler->setEnabled( id, on );
}
