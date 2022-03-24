/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskObjectCounter.h"

#include <qdebug.h>
#include <qset.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qhooks_p.h>
#include <private/qobject_p.h>
#include <private/qquickitem_p.h>
QSK_QT_PRIVATE_END

#define QSK_OBJECT_INFO 0

#if QSK_OBJECT_INFO
#include <qset.h>
#endif

static inline bool qskIsItem( const QObject* object )
{
    QObjectPrivate* o_p = QObjectPrivate::get( const_cast< QObject* >( object ) );

    /*
        The addObject hook is called from the constructor of QObject,
        where we don't have the derived class constructed yet.
        So we can't cast the object itself and also have to rely on
        RTTI being enabled. TODO ...
     */

    return dynamic_cast< QQuickItemPrivate* >( o_p ) != nullptr;
}

namespace
{
    class Counter
    {
      public:
        Counter()
        {
            reset();
        }

        void reset()
        {
            created = destroyed = current = maximum = 0;
        }

        void increment()
        {
            created++;
            current++;

            if ( current > maximum )
                maximum = current;
        }

        void decrement()
        {
            destroyed++;
            current--;
        }

        int created;
        int destroyed;
        int current;
        int maximum;
    };

    class CounterData
    {
      public:
        Counter counter[ 2 ];

#if QSK_OBJECT_INFO
        QSet< const QObject* > objectTable;
#endif
    };

    class CounterHook
    {
      public:
        CounterHook();
        ~CounterHook();

        void registerCounters( CounterData*, bool on );
        bool isCountersRegistered( const CounterData*  ) const;

        bool isActive() const;

        void startup();
        void addObject( QObject* );
        void removeObject( QObject* );

        static void cleanupHook();

      private:
        static void startupHook();
        static void addObjectHook( QObject* );
        static void removeObjectHook( QObject* );

        QSet< CounterData* > m_counterDataSet;

        quintptr m_otherStartup;
        quintptr m_otherAddObject;
        quintptr m_otherRemoveObject;
    };
}

static bool qskAutoDeleteHook = false;
static CounterHook* qskCounterHook = nullptr;

CounterHook::CounterHook()
{
    m_otherStartup = qtHookData[ QHooks::Startup ];
    m_otherAddObject = qtHookData[ QHooks::AddQObject ];
    m_otherRemoveObject = qtHookData[ QHooks::RemoveQObject ];

    qtHookData[ QHooks::Startup ] = reinterpret_cast< quintptr >( &startupHook );
    qtHookData[ QHooks::AddQObject ] = reinterpret_cast< quintptr >( &addObjectHook );
    qtHookData[ QHooks::RemoveQObject ] = reinterpret_cast< quintptr >( &removeObjectHook );
}

CounterHook::~CounterHook()
{
    qtHookData[ QHooks::Startup ] = m_otherStartup;
    qtHookData[ QHooks::AddQObject ] = m_otherAddObject;
    qtHookData[ QHooks::RemoveQObject ] = m_otherRemoveObject;
}

void CounterHook::registerCounters( CounterData* data, bool on )
{
    if ( on )
        m_counterDataSet.insert( data );
    else
        m_counterDataSet.remove( data );
}

bool CounterHook::isCountersRegistered( const CounterData* data ) const
{
    return m_counterDataSet.contains( const_cast< CounterData* >( data ) );
}

bool CounterHook::isActive() const
{
    return !m_counterDataSet.isEmpty();
}

void CounterHook::startup()
{
#if 0
    qDebug() << "** QskObjectCounterHook enabled";
#endif
    if ( m_otherStartup )
        reinterpret_cast< QHooks::StartupCallback >( m_otherStartup )();
}

void CounterHook::addObject( QObject* object )
{
    const bool isItem = qskIsItem( object );

    for ( auto counterData : qAsConst( m_counterDataSet ) )
    {
        counterData->counter[ QskObjectCounter::Objects ].increment();

        if ( isItem )
            counterData->counter[ QskObjectCounter::Items ].increment();

#if QSK_OBJECT_INFO
        counterData->objectTable.insert( object );
#endif
    }

    if ( m_otherAddObject )
        reinterpret_cast< QHooks::AddQObjectCallback >( m_otherAddObject )( object );
}

void CounterHook::removeObject( QObject* object )
{
    const bool isItem = qskIsItem( object );

    for ( auto counterData : qAsConst( m_counterDataSet ) )
    {
        counterData->counter[ QskObjectCounter::Objects ].decrement();

        if ( isItem )
            counterData->counter[ QskObjectCounter::Items ].decrement();

#if QSK_OBJECT_INFO
        counterData->objectTable.remove( object );
#endif
    }

    if ( m_otherRemoveObject )
        reinterpret_cast< QHooks::RemoveQObjectCallback >( m_otherRemoveObject )( object );
}

void CounterHook::startupHook()
{
    if ( qskCounterHook )
        qskCounterHook->startup();
}

void CounterHook::addObjectHook( QObject* object )
{
    if ( qskCounterHook )
        qskCounterHook->addObject( object );
}

void CounterHook::removeObjectHook( QObject* object )
{
    if ( qskCounterHook )
        qskCounterHook->removeObject( object );
}

void CounterHook::cleanupHook()
{
    if ( qskCounterHook && !qskCounterHook->isActive() )
    {
        delete qskCounterHook;
        qskCounterHook = nullptr;
    }

    // From now on we remove the hooks as soon as there are no counters
    qskAutoDeleteHook = true;
}

static void qskInstallCleanupHookHandler()
{
    qAddPostRoutine( CounterHook::cleanupHook );
}

Q_COREAPP_STARTUP_FUNCTION( qskInstallCleanupHookHandler )

class QskObjectCounter::PrivateData
{
  public:
    PrivateData( bool debugAtDestruction )
        : debugAtDestruction( debugAtDestruction )
    {
    }

    CounterData counterData;
    const bool debugAtDestruction;
};

QskObjectCounter::QskObjectCounter( bool debugAtDestruction )
    : m_data( new PrivateData( debugAtDestruction ) )
{
    setActive( true );
}

QskObjectCounter::~QskObjectCounter()
{
    setActive( false );

    if ( m_data->debugAtDestruction )
        dump();
}

void QskObjectCounter::setActive( bool on )
{
    if ( on )
    {
        if ( qskCounterHook == nullptr )
            qskCounterHook = new CounterHook();

        qskCounterHook->registerCounters( &m_data->counterData, on );
    }
    else
    {
        qskCounterHook->registerCounters( &m_data->counterData, on );
        if ( !qskCounterHook->isActive() )
        {
            if ( qskAutoDeleteHook )
            {
                delete qskCounterHook;
                qskCounterHook = nullptr;
            }
        }
    }
}

bool QskObjectCounter::isActive() const
{
    return qskCounterHook && qskCounterHook->isCountersRegistered( &m_data->counterData );
}

void QskObjectCounter::reset()
{
    auto& counters = m_data->counterData.counter;

    counters[ Objects ].reset();
    counters[ Items ].reset();
}

int QskObjectCounter::created( ObjectType objectType ) const
{
    return m_data->counterData.counter[ objectType ].created;
}

int QskObjectCounter::destroyed( ObjectType objectType ) const
{
    return m_data->counterData.counter[ objectType ].destroyed;
}

int QskObjectCounter::current( ObjectType objectType ) const
{
    return m_data->counterData.counter[ objectType ].current;
}

int QskObjectCounter::maximum( ObjectType objectType ) const
{
    return m_data->counterData.counter[ objectType ].maximum;
}

void QskObjectCounter::debugStatistics( QDebug debug, ObjectType objectType ) const
{
    const auto& c = m_data->counterData.counter[ objectType ];

    QDebugStateSaver saver( debug );
    debug.nospace();
    debug << '(';
    debug << "created: " << c.created
          << ", destroyed: " << c.destroyed
          << ", current: " << c.current
          << ", maximum: " << c.maximum;
    debug << ')';

#if QSK_OBJECT_INFO
    if ( objectType == Objects )
    {
        const auto& objectTable = m_data->counterData.objectTable;

        if ( !objectTable.isEmpty() )
        {
            debug << "\n\t=== Leaks ===\n";
            for ( const auto object : objectTable )
            {
                debug << "\tClass: " << object->metaObject()->className();
                if ( !object->objectName().isEmpty() )
                    debug << " Name: " << object->objectName();
                debug << '\n';
            }
        }
    }
#endif
}

void QskObjectCounter::dump() const
{
    QDebug debug = qDebug();

    QDebugStateSaver saver( debug );

    debug.nospace();

    debug << "* Statistics\n";
    debug << "  Objects: ";
    debugStatistics( debug, Objects );

    debug << "\n  Items: ";
    debugStatistics( debug, Items );
}

#ifndef QT_NO_DEBUG_STREAM

QDebug operator<<( QDebug debug, const QskObjectCounter& counter )
{
    counter.debugStatistics( debug, QskObjectCounter::Objects );
    return debug;
}

#endif
