/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskObjectCounter.h"
#include <QQuickItem>
#include <QSet>
#include <QDebug>

QSK_QT_PRIVATE_BEGIN
#include <private/qhooks_p.h>
#include <private/qquickitem_p.h>
#include <private/qobject_p.h>
QSK_QT_PRIVATE_END

static inline bool qskIsItem( const QObject* object )
{
    QObjectPrivate* o_p = QObjectPrivate::get( const_cast< QObject* >( object ) );

    // The addObject hook is called from the constructor of QObject,
    // where we don't have the derived class constructed yet.
    // So we can't cast the object itself.

    return dynamic_cast< QQuickItemPrivate* >( o_p ) != nullptr;
}

class QskObjectCounterHook
{
public:
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

    void startup() const
    {
#if 0
        qDebug() << "** QskObjectCounterHook enabled";
#endif
    }

    void addObject( QObject* object )
    {
        for ( auto counter : m_counterSet )
            counter->addObject( object );
    }

    void removeObject( QObject* object )
    {
        for ( auto counter : m_counterSet )
            counter->removeObject( object );
    }

private:
    QSet< QskObjectCounter* > m_counterSet;
};

static QskObjectCounterHook* qskCounterHook();

static void qskStartupHook()
{
    qskCounterHook()->startup();
}

static void qskAddObjectHook( QObject* object )
{
    qskCounterHook()->addObject( object );
}

static void qskRemoveObjectHook( QObject* object )
{
    qskCounterHook()->removeObject( object );
}

static QskObjectCounterHook* qskCounterHook()
{
    static QskObjectCounterHook* hook = nullptr;

    if ( hook == nullptr )
    {
        hook = new QskObjectCounterHook(); // never deleted !

        qtHookData[QHooks::Startup] = reinterpret_cast< quintptr >( &qskStartupHook );
        qtHookData[QHooks::AddQObject] = reinterpret_cast< quintptr >( &qskAddObjectHook );
        qtHookData[QHooks::RemoveQObject] = reinterpret_cast< quintptr >( &qskRemoveObjectHook );
    }

    return hook;
}

QskObjectCounter::QskObjectCounter( bool debugAtDestruction ):
    m_debugAtDestruction( debugAtDestruction )
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
    qskCounterHook()->registerCounter( this, on );
}

bool QskObjectCounter::isActive() const
{
    return qskCounterHook()->isCounterRegistered( this );
}

void QskObjectCounter::addObject( QObject* object )
{
    m_counter[Objects].increment();

    if ( qskIsItem( object ) )
        m_counter[Items].increment();
}

void QskObjectCounter::removeObject( QObject* object )
{
    m_counter[Objects].decrement();

    if ( qskIsItem( object ) )
        m_counter[Items].decrement();
}

void QskObjectCounter::reset()
{
    m_counter[Objects].reset();
    m_counter[Items].reset();
}

int QskObjectCounter::created( ObjectType objectType ) const
{
    return m_counter[objectType].created;
}

int QskObjectCounter::destroyed( ObjectType objectType ) const
{
    return m_counter[objectType].destroyed;
}

int QskObjectCounter::current( ObjectType objectType ) const
{
    return m_counter[objectType].current;
}

int QskObjectCounter::maximum( ObjectType objectType ) const
{
    return m_counter[objectType].maximum;
}

void QskObjectCounter::debugStatistics( QDebug debug, ObjectType objectType ) const
{
    const Counter& c = m_counter[objectType];

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

