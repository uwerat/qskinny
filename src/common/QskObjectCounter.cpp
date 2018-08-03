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

static void qskStartupHook();
static void qskAddObjectHook( QObject* );
static void qskRemoveObjectHook( QObject* );

class QskObjectCounterHook
{
  public:
    QskObjectCounterHook()
    {
        m_otherStartup = qtHookData[ QHooks::Startup ];
        m_otherAddObject = qtHookData[ QHooks::AddQObject ];
        m_otherRemoveObject = qtHookData[ QHooks::RemoveQObject ];

        qtHookData[ QHooks::Startup ] = reinterpret_cast< quintptr >( &qskStartupHook );
        qtHookData[ QHooks::AddQObject ] = reinterpret_cast< quintptr >( &qskAddObjectHook );
        qtHookData[ QHooks::RemoveQObject ] = reinterpret_cast< quintptr >( &qskRemoveObjectHook );
    }

    ~QskObjectCounterHook()
    {
        qtHookData[ QHooks::Startup ] = m_otherStartup;
        qtHookData[ QHooks::AddQObject ] = m_otherAddObject;
        qtHookData[ QHooks::RemoveQObject ] = m_otherRemoveObject;
    }

    void registerCounter( QskObjectCounter* counter, bool on )
    {
        if ( on )
            m_counterSet.insert( counter );
        else
            m_counterSet.remove( counter );
    }

    bool isCounterRegistered( const QskObjectCounter* counter ) const
    {
        return m_counterSet.contains( const_cast< QskObjectCounter* >( counter ) );
    }

    bool isActive() const
    {
        return !m_counterSet.isEmpty();
    }

    void startup()
    {
#if 0
        qDebug() << "** QskObjectCounterHook enabled";
#endif
        if ( m_otherStartup )
            reinterpret_cast< QHooks::StartupCallback >( m_otherStartup )();
    }

    void addObject( QObject* object )
    {
        for ( auto counter : qskAsConst( m_counterSet ) )
            counter->addObject( object );

        if ( m_otherAddObject )
            reinterpret_cast< QHooks::AddQObjectCallback >( m_otherAddObject )( object );
    }

    void removeObject( QObject* object )
    {
        for ( auto counter : qskAsConst( m_counterSet ) )
            counter->removeObject( object );

        if ( m_otherRemoveObject )
            reinterpret_cast< QHooks::RemoveQObjectCallback >( m_otherRemoveObject )( object );
    }

    static bool autoDelete;

  private:
    QSet< QskObjectCounter* > m_counterSet;

    quintptr m_otherStartup;
    quintptr m_otherAddObject;
    quintptr m_otherRemoveObject;
};

bool QskObjectCounterHook::autoDelete = false;
static QskObjectCounterHook* qskCounterHook = nullptr;

static void qskStartupHook()
{
    if ( qskCounterHook )
        qskCounterHook->startup();
}

static void qskAddObjectHook( QObject* object )
{
    if ( qskCounterHook )
        qskCounterHook->addObject( object );
}

static void qskRemoveObjectHook( QObject* object )
{
    if ( qskCounterHook )
        qskCounterHook->removeObject( object );
}

static void qskCleanupHook()
{
    if ( qskCounterHook && !qskCounterHook->isActive() )
    {
        delete qskCounterHook;
        qskCounterHook = nullptr;
    }

    // From now on we remove the hooks as soon as there are no counters
    QskObjectCounterHook::autoDelete = true;
}

static void qskInstallCleanupHookHandler()
{
    qAddPostRoutine( qskCleanupHook );
}

Q_COREAPP_STARTUP_FUNCTION( qskInstallCleanupHookHandler )

QskObjectCounter::QskObjectCounter( bool debugAtDestruction )
    : m_debugAtDestruction( debugAtDestruction )
{
    setActive( true );
}

QskObjectCounter::~QskObjectCounter()
{
    setActive( false );

    if ( m_debugAtDestruction )
        dump();
}

void QskObjectCounter::setActive( bool on )
{
    if ( on )
    {
        if ( qskCounterHook == nullptr )
            qskCounterHook = new QskObjectCounterHook();

        qskCounterHook->registerCounter( this, on );
    }
    else
    {
        qskCounterHook->registerCounter( this, on );
        if ( !qskCounterHook->isActive() )
        {
            if ( QskObjectCounterHook::autoDelete )
            {
                delete qskCounterHook;
                qskCounterHook = nullptr;
            }
        }
    }
}

bool QskObjectCounter::isActive() const
{
    return qskCounterHook && qskCounterHook->isCounterRegistered( this );
}

void QskObjectCounter::addObject( QObject* object )
{
    m_counter[ Objects ].increment();

    if ( qskIsItem( object ) )
        m_counter[ Items ].increment();
}

void QskObjectCounter::removeObject( QObject* object )
{
    m_counter[ Objects ].decrement();

    if ( qskIsItem( object ) )
        m_counter[ Items ].decrement();
}

void QskObjectCounter::reset()
{
    m_counter[ Objects ].reset();
    m_counter[ Items ].reset();
}

int QskObjectCounter::created( ObjectType objectType ) const
{
    return m_counter[ objectType ].created;
}

int QskObjectCounter::destroyed( ObjectType objectType ) const
{
    return m_counter[ objectType ].destroyed;
}

int QskObjectCounter::current( ObjectType objectType ) const
{
    return m_counter[ objectType ].current;
}

int QskObjectCounter::maximum( ObjectType objectType ) const
{
    return m_counter[ objectType ].maximum;
}

void QskObjectCounter::debugStatistics( QDebug debug, ObjectType objectType ) const
{
    const Counter& c = m_counter[ objectType ];

    QDebugStateSaver saver( debug );
    debug.nospace();
    debug << '(';
    debug << "created: " << c.created
          << ", destroyed: " << c.destroyed
          << ", current: " << c.current
          << ", maximum: " << c.maximum;
    debug << ')';
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
