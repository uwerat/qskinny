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

static inline QShortcutMap& qskShortcutMap()
{
    return QGuiApplicationPrivate::instance()->shortcutMap;
}

class QskShortcutHandler final : public QObject
{
public:
    QskShortcutHandler();

    int add( QQuickItem*, const QKeySequence&,
        const QObject* receiver, const char* method );

    int add( QQuickItem*, const QKeySequence&,
        const QObject* receiver, QtPrivate::QSlotObjectBase* );

    void remove( int id );

    void setEnabled( int id, bool );
    void setAutoRepeat( int id, bool repeat );

    virtual bool eventFilter( QObject*, QEvent* ) override final;

private:
    int insert( QQuickItem*, const QKeySequence&,
        const QObject* receiver, const QMetaMethod&, QtPrivate::QSlotObjectBase* );

    void cleanUp( QObject* );

    static bool contextMatcher( QObject*, Qt::ShortcutContext );

    class InvokeData
    {
    public:
        InvokeData():
            item( nullptr ),
            receiver( nullptr ),
            slotObject( nullptr )
        {
        }

        ~InvokeData()
        {
            if ( slotObject )
                slotObject->destroyIfLastRef();
        }

        QQuickItem* item;
        const QObject* receiver;
        QMetaMethod method;
        QtPrivate::QSlotObjectBase* slotObject;
    };

    std::map< int, InvokeData > m_invokeDataMap;
};

Q_GLOBAL_STATIC( QskShortcutHandler, qskShortcutHandler )

QskShortcutHandler::QskShortcutHandler()
{
    installEventFilter( this );
}

int QskShortcutHandler::add(
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
            id = insert( item, key,
                receiver, metaObject->method( methodIndex ), nullptr );
        }
    }

    return id;
}

int QskShortcutHandler::add( QQuickItem* item, const QKeySequence& key,
    const QObject* receiver, QtPrivate::QSlotObjectBase* slotObj )
{
    return insert( item, key, receiver, QMetaMethod(), slotObj );
}

int QskShortcutHandler::insert(
    QQuickItem* item, const QKeySequence& key,
    const QObject* receiver, const QMetaMethod& method,
    QtPrivate::QSlotObjectBase* slotObject )
{
    if ( receiver )
    {
        receiver->disconnect( this );
        connect( receiver, &QObject::destroyed, this, &QskShortcutHandler::cleanUp );
    }

    int id = 0;

    auto& map = qskShortcutMap();

    if ( item )
    {
        if ( item != receiver )
        {
            item->disconnect( this );
            connect( item, &QObject::destroyed, this, &QskShortcutHandler::cleanUp );
        }

        id = map.addShortcut( item, key, Qt::WindowShortcut, contextMatcher );
    }
    else
    {
        id = map.addShortcut( this, key, Qt::ApplicationShortcut, contextMatcher );
    }

    auto& data = m_invokeDataMap[ id ];

    data.item = item;
    data.receiver = receiver;

    if ( slotObject )
        data.slotObject = slotObject;
    else
        data.method = method;

    return id;
}

void QskShortcutHandler::remove( int id )
{
    auto it = m_invokeDataMap.find( id );
    if ( it == m_invokeDataMap.end() )
        return;

    auto& map = qskShortcutMap();
    map.removeShortcut( id, nullptr );

    const QQuickItem* item = it->second.item;
    const QObject* receiver = it->second.receiver;

    m_invokeDataMap.erase( it );

    /*
        Finally let's check if we can disconnect
        from the destroyed signals
     */
    for ( const auto& entry : qskAsConst( m_invokeDataMap ) )
    {
        if ( item == nullptr && receiver == nullptr )
            break;

        if ( entry.second.item == item )
            item = nullptr;

        if ( entry.second.receiver == receiver )
            receiver = nullptr;
    }

    if ( item )
        item->disconnect( this );

    if ( receiver && receiver != item )
        receiver->disconnect( this );
}

void QskShortcutHandler::cleanUp( QObject* object )
{
    /*
        When item != receiver we might remain being connected
        to destroyed signals we are not interested in.
     */
    auto& map = qskShortcutMap();

    for ( auto it = m_invokeDataMap.begin(); it != m_invokeDataMap.end(); )
    {
        const auto& data = it->second;

        if ( data.item == object || data.receiver == object )
        {
            map.removeShortcut( it->first, nullptr );
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

                item = item->parentItem();
            }

            // we want to process the following QShortcutEvent
            object->installEventFilter( qskShortcutHandler );
            return true;
        }
    }

    return false;
}

void QskShortcutHandler::setEnabled( int id, bool enabled )
{
    auto& map = qskShortcutMap();
    map.setShortcutEnabled( enabled, id, this );
}

void QskShortcutHandler::setAutoRepeat( int id, bool repeat )
{
    auto& map = qskShortcutMap();
    map.setShortcutAutoRepeat( repeat, id, this );
}

bool QskShortcutHandler::eventFilter( QObject* object, QEvent* event )
{
    if ( event->type() != QEvent::Shortcut )
        return false;

    if ( object != this )
        object->removeEventFilter( this );

    const QShortcutEvent* se = static_cast< const QShortcutEvent* >( event );

#if 0
    // do we want to handle this ???
    if ( se->isAmbiguous() )
        ....
#endif

    const auto it = m_invokeDataMap.find( se->shortcutId() );
    if ( it != m_invokeDataMap.end() )
    {
        const auto& invokeData = it->second;

        Q_ASSERT( invokeData.item == nullptr || invokeData.item == object );

        auto receiver = const_cast< QObject* >( invokeData.receiver );

        if ( invokeData.slotObject )
        {
            void* args[] = { 0 };

            if ( receiver && receiver->thread() != thread() )
            {
                QCoreApplication::postEvent( receiver,
                    new QMetaCallEvent( invokeData.slotObject, nullptr, 0, 0, nullptr, args ) );
            }
            else
            {
                invokeData.slotObject->call( receiver, args );
            }
        }
        else
        {
            invokeData.method.invoke( receiver, Qt::AutoConnection );
        }

        return true;
    }

    // seems like someone else is also interested in shortcuts
    return false;
}

int QskShortcut::addMethod( QQuickItem* item, const QKeySequence& key,
    bool autoRepeat, const QObject* receiver, const char* method )
{
    if ( receiver == nullptr )
    {
        return 0;
    }

    int id = qskShortcutHandler->add( item, key, receiver, method );
    if ( id && !autoRepeat )
        qskShortcutHandler->setAutoRepeat( id, false );

    return id;
}

int QskShortcut::addSlotObject( QQuickItem* item, const QKeySequence& key,
    bool autoRepeat, const QObject* receiver, QtPrivate::QSlotObjectBase* slotObject )
{
    int id = qskShortcutHandler->add( item, key, receiver, slotObject );
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

void QskShortcut::removeShortcut( int id )
{
    qskShortcutHandler->remove( id );
}
