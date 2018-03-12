/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskShortcutMap.h"
#include "QskControl.h"
#include "QskMetaInvokable.h"

#include <QQuickWindow>
#include <QKeySequence>
#include <QGlobalStatic>
#include <QtGui/private/qguiapplication_p.h>

#include <map>

static inline QShortcutMap* qskShortcutMap()
{
    return qGuiApp ? &QGuiApplicationPrivate::instance()->shortcutMap : nullptr;
}

class QskShortcutHandler final : public QObject
{
public:
    QskShortcutHandler();

    int insert( QQuickItem*, const QKeySequence&, bool autoRepeat,
        const QObject*, const QskMetaInvokable& );

    void remove( int id );

    void setEnabled( int id, bool );
    void setAutoRepeat( int id, bool repeat );

    virtual bool eventFilter( QObject*, QEvent* ) override final;

private:

    void cleanUp( QObject* );

    class InvokeData
    {
    public:
        InvokeData():
            item( nullptr ),
            receiver( nullptr )
        {
        }

        QQuickItem* item;
        const QObject* receiver;
        QskMetaInvokable invokable;
    };

    std::map< int, InvokeData > m_invokeDataMap;
};

Q_GLOBAL_STATIC( QskShortcutHandler, qskShortcutHandler )

static bool qskContextMatcher( QObject* object, Qt::ShortcutContext context )
{
    if ( context == Qt::ApplicationShortcut )
        return true;

    auto item = qobject_cast< QQuickItem* >( object );
    if ( item && context == Qt::WindowShortcut )
    {
        if ( QskShortcutMap::contextMatcher( item, context ) )
        {
            /*
                Unfortunatley there is no way to have to know about
                the contextItem without making it the receiver of
                the following QShortcutEvent. So we have to install
                an event handler to process and swallow it in QskShortcutHandler.
             */

            item->installEventFilter( qskShortcutHandler );
            return true;
        }
    }

    return false;
}

QskShortcutHandler::QskShortcutHandler()
{
    // to process all sort of shortcut events at the same place
    installEventFilter( this );
}

int QskShortcutHandler::insert(
    QQuickItem* item, const QKeySequence& sequence, bool autoRepeat,
    const QObject* receiver, const QskMetaInvokable& invokable )
{
    if ( sequence.isEmpty() )
    {
        qDebug() << "QskShortcutMap: invalid shortcut key sequence";
        return 0;
    }

#if 1
    // should be a compile time check for functor based slots
    if ( invokable.parameterCount() > 0 )
    {
        qDebug() << "QskShortcutMap: invalid slot parameter count";
        return 0;
    }
#endif

    if ( receiver )
    {
        connect( receiver, &QObject::destroyed,
            this, &QskShortcutHandler::cleanUp, Qt::UniqueConnection );
    }

    int id = 0;

    auto map = qskShortcutMap();

    if ( item )
    {
        if ( item != receiver )
        {
            connect( item, &QObject::destroyed,
                this, &QskShortcutHandler::cleanUp, Qt::UniqueConnection );
        }

        id = map->addShortcut( item, sequence, Qt::WindowShortcut, qskContextMatcher );
    }
    else
    {
        id = map->addShortcut( this, sequence, Qt::ApplicationShortcut, qskContextMatcher );
    }

    auto& data = m_invokeDataMap[ id ];

    data.item = item;
    data.receiver = receiver;
    data.invokable = invokable;

    if ( !autoRepeat )
        setAutoRepeat( id, false );

    return id;
}

void QskShortcutHandler::remove( int id )
{
    auto it = m_invokeDataMap.find( id );
    if ( it == m_invokeDataMap.end() )
        return;

    auto map = qskShortcutMap();
    map->removeShortcut( id, nullptr );

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
        to destroyed signals we are not interested in anymore. TODO ...
     */
    auto map = qskShortcutMap();

    for ( auto it = m_invokeDataMap.begin(); it != m_invokeDataMap.end(); )
    {
        const auto& data = it->second;

        if ( data.item == object || data.receiver == object )
        {
            if ( map )
                map->removeShortcut( it->first, nullptr );

            it = m_invokeDataMap.erase( it );

            continue;
        }

        ++it;
    }
}

void QskShortcutHandler::setEnabled( int id, bool enabled )
{
    auto map = qskShortcutMap();
    map->setShortcutEnabled( enabled, id, this );
}

void QskShortcutHandler::setAutoRepeat( int id, bool repeat )
{
    auto map = qskShortcutMap();
    map->setShortcutAutoRepeat( repeat, id, this );
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
        auto& data = it->second;

        Q_ASSERT( data.item == nullptr || data.item == object );

        auto receiver = const_cast< QObject* > ( data.receiver );
        void* args[] = { nullptr };

        data.invokable.invoke( receiver, args, Qt::AutoConnection );

        return true;
    }

    // seems like someone else is also interested in shortcuts
    return false;
}

int QskShortcutMap::addMethod( QQuickItem* item, const QKeySequence& sequence,
    bool autoRepeat, const QObject* receiver, const char* method )
{
    if ( receiver == nullptr )
    {
        qDebug() << "QskShortcutMap: bad receiver for shortcut:" << sequence;
        return 0;
    }

    return qskShortcutHandler->insert(
        item, sequence, autoRepeat, receiver, qskMetaMethod( receiver, method ) );
}

int QskShortcutMap::addFunction( QQuickItem* item, const QKeySequence& sequence,
    bool autoRepeat, const QObject* receiver, const QskMetaFunction& function )
{
    if ( ( receiver == nullptr )
        && ( function.functionType() == QskMetaFunction::Member ) )
    {
        qDebug() << "QskShortcutMap: bad receiver for shortcut:" << sequence;
        return 0;
    }

    return qskShortcutHandler->insert(
        item, sequence, autoRepeat, receiver, function );
}

void QskShortcutMap::setAutoRepeat( int id, bool on )
{
    qskShortcutHandler->setAutoRepeat( id, on );
}

void QskShortcutMap::setEnabled( int id, bool on )
{
    qskShortcutHandler->setEnabled( id, on );
}

void QskShortcutMap::removeShortcut( int id )
{
    qskShortcutHandler->remove( id );
}

bool QskShortcutMap::contextMatcher(
    const QQuickItem* item, Qt::ShortcutContext context )
{
    if ( context == Qt::ApplicationShortcut )
        return true;

    if ( item && context == Qt::WindowShortcut )
    {
        const auto focusWindow = QGuiApplication::focusWindow();

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
            if ( qskIsShortcutScope( item ) )
            {
                if ( !item->hasFocus() )
                    return false;
            }

            item = item->parentItem();
        }

        return true;
    }

    return false;
}

