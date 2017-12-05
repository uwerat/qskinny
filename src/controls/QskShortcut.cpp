/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskShortcut.h"
#include "QskControl.h"

#include <QQuickWindow>
#include <QMetaMethod>
#include <QKeySequence>
#include <QGlobalStatic>
#include <QtGui/private/qguiapplication_p.h>

#include <map>

class QskShortcutHandler final : public QObject
{
public:
    QskShortcutHandler();

    int addShortcut( QQuickItem*, const QKeySequence&,
        const QObject* receiver, const char* method );

    int addShortcut( QQuickItem*, const QKeySequence&,
        const QObject* receiver, QtPrivate::QSlotObjectBase* );

    void setEnabled( int id, bool );
    void setAutoRepeat( int id, bool repeat );

    virtual bool eventFilter( QObject*, QEvent* ) override final;

private:
    int addShortcut( QQuickItem*, const QKeySequence&,
        const QObject* receiver, const QMetaMethod&, QtPrivate::QSlotObjectBase* );

    void cleanUp( QObject* );

    static bool contextMatcher( QObject*, Qt::ShortcutContext );
    static QShortcutMap& map();

    class Data
    {
    public:
        Data():
            item( nullptr ),
            receiver( nullptr ),
            slotObj( nullptr )
        {
        }

        ~Data()
        {
            if ( slotObj )
                slotObj->destroyIfLastRef();
        }

        QQuickItem* item;
        const QObject* receiver;
        QMetaMethod method;
        QtPrivate::QSlotObjectBase* slotObj;
    };

    std::map< int, Data > m_invokeDataMap;
};

QskShortcutHandler::QskShortcutHandler()
{
    installEventFilter( this );
}

inline QShortcutMap& QskShortcutHandler::map()
{
    return QGuiApplicationPrivate::instance()->shortcutMap;
}

int QskShortcutHandler::addShortcut(
    QQuickItem* item, const QKeySequence& key,
    const QObject* receiver, const char* method )
{
    int id = 0;

    if ( receiver )
    {
        const QMetaObject* metaObject = receiver->metaObject();

        const int methodIndex = metaObject->indexOfMethod(
            QMetaObject::normalizedSignature( method ).constData() + 1 );

        if ( methodIndex >= 0 )
        {
            id = addShortcut( item, key,
                receiver, metaObject->method( methodIndex ), nullptr );
        }
    }

    return id;
}

int QskShortcutHandler::addShortcut( QQuickItem* item, const QKeySequence& key,
    const QObject* receiver, QtPrivate::QSlotObjectBase* slotObj )
{
    return addShortcut( item, key, receiver, QMetaMethod(), slotObj );
}

int QskShortcutHandler::addShortcut(
    QQuickItem* item, const QKeySequence& key,
    const QObject* receiver, const QMetaMethod& method,
    QtPrivate::QSlotObjectBase* slotObj )
{
    if ( receiver == nullptr )
        return 0;

#if 1
    // multiple connections ????
    connect( receiver, &QObject::destroyed, this, &QskShortcutHandler::cleanUp );
#endif

    int id = 0;

    if ( item )
    {
        item->installEventFilter( this );
        if ( item != receiver )
            connect( item, &QObject::destroyed, this, &QskShortcutHandler::cleanUp );

        id = map().addShortcut( item, key, Qt::WindowShortcut, contextMatcher );
    }
    else
    {
        id = map().addShortcut( this, key, Qt::ApplicationShortcut, contextMatcher );
    }

    auto& data = m_invokeDataMap[ id ];

    data.item = item;
    data.receiver = receiver;

    if ( slotObj )
        data.slotObj = slotObj;
    else
        data.method = method;

    return id;
}

void QskShortcutHandler::cleanUp( QObject* object )
{
    for ( auto it = m_invokeDataMap.begin(); it != m_invokeDataMap.end(); )
    {
        const auto& data = it->second;

        if ( data.receiver == object || data.item == object  )
        {
            map().removeShortcut( it->first, this );
            it = m_invokeDataMap.erase( it );

            continue;
        }

        ++it;
    }
}

bool QskShortcutHandler::contextMatcher( QObject* object, Qt::ShortcutContext context )
{
    if ( context == Qt::ApplicationShortcut )
        return true;

    if ( context == Qt::WindowShortcut )
    {
        const auto focusWindow = QGuiApplication::focusWindow();

        if ( auto item = qobject_cast< const QQuickItem* >( object ) )
        {
            const auto window = item->window();
            if ( window == nullptr || window != focusWindow )
            {
                return false;
            }

            while ( item )
            {
                /*
                    We have to find out if the active focus is inside
                    the surronding shortcut scope.
                 */
                if ( QskControl::isShortcutScope( item ) )
                {
                    if ( !item->hasFocus() )
                        return false;
                }
            }

            item = item->parentItem();
        }
    }

    return false;
}

void QskShortcutHandler::setEnabled( int id, bool enabled )
{
    map().setShortcutEnabled( enabled, id, this );
}

void QskShortcutHandler::setAutoRepeat( int id, bool repeat )
{
    map().setShortcutAutoRepeat( repeat, id, this );
}

bool QskShortcutHandler::eventFilter( QObject* object, QEvent* event )
{
    if ( event->type() != QEvent::Shortcut )
        return false;

    const QShortcutEvent* se = static_cast< const QShortcutEvent* >( event );

#if 0
    // do we want to handle this ???
    if ( se->isAmbiguous() )
        ....
#endif

    const auto it = m_invokeDataMap.find( se->shortcutId() );
    if ( it != m_invokeDataMap.end() )
    {
        const Data& data = it->second;

        Q_ASSERT( data.item == nullptr || data.item == object );

        auto receiver = const_cast< QObject* >( data.receiver );

        if ( data.slotObj )
        {
            void* args[] = { 0 };

            if ( receiver && receiver->thread() != thread() )
            {
                QCoreApplication::postEvent( receiver,
                    new QMetaCallEvent( data.slotObj, nullptr, 0, 0, nullptr, args ) );
            }
            else
            {
                data.slotObj->call( receiver, args );
            }
        }
        else
        {
            data.method.invoke( receiver, Qt::AutoConnection );
        }

        return true;
    }

    // seems like someone else is also interested in shortcuts
    return false;
}

Q_GLOBAL_STATIC( QskShortcutHandler, qskShortcutHandler )

int QskShortcut::addShortcut( const QKeySequence& key,
    bool autoRepeat, const QObject* receiver, const char* method )
{
    QQuickWindow* window = nullptr;
    return addShortcut( window, key, autoRepeat, receiver, method );
}

int QskShortcut::addShortcut( QQuickWindow* window, const QKeySequence& key,
    bool autoRepeat, const QObject* receiver, const char* method )
{
    auto item = window ? window->contentItem() : nullptr;

    int id = qskShortcutHandler->addShortcut( item, key, receiver, method );
    if ( id && !autoRepeat )
        qskShortcutHandler->setAutoRepeat( id, false );

    return id;
}

int QskShortcut::addShortcut( QQuickItem* item, const QKeySequence& key,
    bool autoRepeat, const QObject* receiver, const char* method )
{
    int id = qskShortcutHandler->addShortcut( item, key, receiver, method );
    if ( id && !autoRepeat )
        qskShortcutHandler->setAutoRepeat( id, false );

    return id;
}

int QskShortcut::addShortcutImpl( const QKeySequence& key,
    bool autoRepeat, const QObject* receiver, QtPrivate::QSlotObjectBase* slotObj )
{
    int id = qskShortcutHandler->addShortcut( nullptr, key, receiver, slotObj );
    if ( id && !autoRepeat )
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
