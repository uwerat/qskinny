/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskAnimator.h"

#include <qelapsedtimer.h>
#include <qglobalstatic.h>
#include <qobject.h>
#include <qquickwindow.h>
#include <qvector.h>

#include <cmath>

#ifndef QT_NO_DEBUG_STREAM
#include <qdebug.h>
#endif

namespace
{
    class Statistics
    {
      public:
        inline Statistics()
        {
            reset();
        }

#ifndef QT_NO_DEBUG_STREAM
        void debugStatistics( QDebug debug )
        {
            QDebugStateSaver saver( debug );
            debug.nospace();
            debug << '(';
            debug << "created: " << created
                  << ", destroyed: " << destroyed
                  << ", current: " << current
                  << ", maximum: " << maximum;
            debug << ')';
        }
#endif

        inline void reset()
        {
            created = destroyed = current = maximum = 0;
        }

        inline void increment()
        {
            created++;
            current++;

            if ( current > maximum )
                maximum = current;
        }

        inline void decrement()
        {
            destroyed++;
            current--;
        }

        int created;
        int destroyed;
        int current;
        int maximum;
    };
}

/*
    We need to have at least one QObject to connect to QQuickWindow
    updates - but then we can advance the animators manually without
    making them heavy QObjects too.
 */
class QskAnimatorDriver final : public QObject
{
    Q_OBJECT

  public:
    QskAnimatorDriver();

    void registerAnimator( QskAnimator* );
    void unregisterAnimator( QskAnimator* );

    qint64 referenceTime() const;

  Q_SIGNALS:
    void advanced( QQuickWindow* );
    void terminated( QQuickWindow* );

  private:
    void advanceAnimators( QQuickWindow* );
    void removeWindow( QQuickWindow* );
    void scheduleUpdate( QQuickWindow* );

    QElapsedTimer m_referenceTime;

    // a sorted vector, good for iterating and good enough for look ups
    QVector< QskAnimator* > m_animators;

    /*
       Having a more than a very few windows with running animators is
       very unlikely and using a hash table instead of a vector probably
       creates more overhead than being good for something.
     */
    QVector< QQuickWindow* > m_windows;
    mutable int m_index; // current value, when iterating
};

QskAnimatorDriver::QskAnimatorDriver()
    : m_index( -1 )
{
    m_referenceTime.start();
}

inline qint64 QskAnimatorDriver::referenceTime() const
{
    return m_referenceTime.elapsed();
}

void QskAnimatorDriver::registerAnimator( QskAnimator* animator )
{
    Q_ASSERT( animator->window() );

    // do we want to be thread safe ???

    auto it = std::lower_bound( m_animators.begin(), m_animators.end(), animator );
    if ( it != m_animators.end() && *it == animator )
        return;

    if ( m_index > 0 )
    {
        if ( it - m_animators.begin() < m_index )
            m_index++;
    }

    m_animators.insert( it, animator );

    if ( auto window = animator->window() )
    {
        if ( !m_windows.contains( window ) )
        {
            m_windows += window;

            connect( window, &QQuickWindow::afterAnimating,
                this, [ this, window ]() { advanceAnimators( window ); } );

            connect( window, &QQuickWindow::frameSwapped,
                this, [ this, window ]() { scheduleUpdate( window ); } );

            connect( window, &QWindow::visibleChanged,
                this, [ this, window ]( bool on ) { if ( !on ) removeWindow( window ); } );

            connect( window, &QObject::destroyed,
                this, [ this, window ]( QObject* ) { removeWindow( window ); } );

            window->update();
        }
    }
}

void QskAnimatorDriver::scheduleUpdate( QQuickWindow* window )
{
    if ( m_windows.contains( window ) )
        window->update();
}

void QskAnimatorDriver::removeWindow( QQuickWindow* window )
{
    window->disconnect( this );
    m_windows.removeOne( window );

    for ( auto it = m_animators.begin(); it != m_animators.end(); )
    {
        if ( ( *it )->window() == window )
            it = m_animators.erase( it );
        else
            ++it;
    }
}

void QskAnimatorDriver::unregisterAnimator( QskAnimator* animator )
{
    auto it = std::find( m_animators.begin(), m_animators.end(), animator );
    if ( it != m_animators.end() )
    {
        if ( it - m_animators.begin() < m_index )
            m_index--;

        m_animators.erase( it );
    }
}

void QskAnimatorDriver::advanceAnimators( QQuickWindow* window )
{
    bool hasAnimators = false;
    bool hasTerminations = false;

    for ( m_index = m_animators.size() - 1; m_index >= 0; m_index-- )
    {
        // Advancing animators might create/remove animators, what is handled by
        // adjusting m_index in register/unregister

        auto animator = m_animators[ m_index ];
        if ( animator->window() == window )
        {
            hasAnimators = true;

            if ( animator->isRunning() )
            {
                animator->update();

                if ( !animator->isRunning() )
                    hasTerminations = true;
            }
        }
    }

    m_index = -1;

    if ( !hasAnimators )
    {
        window->disconnect( this );
        m_windows.removeOne( const_cast< QQuickWindow* >( window ) );
    }

    Q_EMIT advanced( window );

    if ( hasTerminations )
        Q_EMIT terminated( window );
}

Q_GLOBAL_STATIC( QskAnimatorDriver, qskAnimatorDriver )
Q_GLOBAL_STATIC( Statistics, qskStatistics )

QskAnimator::QskAnimator()
    : m_window( nullptr )
    , m_duration( 200 )
    , m_startTime( -1 )
{
    if ( qskStatistics )
        qskStatistics->increment();
}

QskAnimator::~QskAnimator()
{
    if ( qskAnimatorDriver )
        qskAnimatorDriver->unregisterAnimator( this );

    if ( qskStatistics )
        qskStatistics->decrement();
}

QQuickWindow* QskAnimator::window() const
{
    return m_window;
}

void QskAnimator::setWindow( QQuickWindow* window )
{
    if ( window != m_window )
    {
        stop();
        m_window = window;
    }
}

void QskAnimator::setAutoRepeat( bool on )
{
    m_autoRepeat = on;
}

void QskAnimator::setDuration( int ms )
{
    m_duration = ms;
}

void QskAnimator::setEasingCurve( QEasingCurve::Type type )
{
    if ( type >= 0 && type < QEasingCurve::Custom )
    {
        // initialize a static curve table once and then reuse
        // it, instead of recreating a new curve each time.

        static QEasingCurve curveTable[ QEasingCurve::Custom ];
        if ( curveTable[ type ].type() != type )
            curveTable[ type ].setType( type );

        m_easingCurve = curveTable[ type ];
    }
    else
    {
        m_easingCurve.setType( type );
    }
}

void QskAnimator::setEasingCurve( const QEasingCurve& easingCurve )
{
    m_easingCurve = easingCurve;
}

const QEasingCurve& QskAnimator::easingCurve() const
{
    return m_easingCurve;
}

qint64 QskAnimator::elapsed() const
{
    if ( !isRunning() )
        return -1;

    const qint64 driverTime = qskAnimatorDriver->referenceTime();
    return driverTime - m_startTime;
}

void QskAnimator::start()
{
    if ( isRunning() )
        return;

    if ( auto driver = qskAnimatorDriver )
    {
        driver->registerAnimator( this );
        m_startTime = driver->referenceTime();

        setup();
    }
}

void QskAnimator::stop()
{
    if ( !isRunning() )
        return;

    if ( auto driver = qskAnimatorDriver )
        driver->unregisterAnimator( this );

    m_startTime = -1;
    done();
}

void QskAnimator::update()
{
    if ( !isRunning() )
        return;

    const qint64 driverTime = qskAnimatorDriver->referenceTime();

    if ( m_autoRepeat )
    {
        double progress = std::fmod( driverTime - m_startTime, m_duration );
        progress /= m_duration;

        advance( m_easingCurve.valueForProgress( progress ) );
    }
    else
    {
        double progress = ( driverTime - m_startTime ) / double( m_duration );

        if ( progress > 1.0 )
            progress = 1.0;

        advance( m_easingCurve.valueForProgress( progress ) );

        if ( progress >= 1.0 )
            stop();
    }
}

void QskAnimator::setup()
{
    // nop
}

void QskAnimator::done()
{
    // nop
}

#if 1
// we should also have functor based callbacks. TODO ...
#endif

QMetaObject::Connection QskAnimator::addCleanupHandler(
    QObject* receiver, const char* method, Qt::ConnectionType type )
{
    return QObject::connect( qskAnimatorDriver,
        SIGNAL(terminated(QQuickWindow*)), receiver, method, type );
}

QMetaObject::Connection QskAnimator::addAdvanceHandler(
    QObject* receiver, const char* method, Qt::ConnectionType type )
{
    return QObject::connect( qskAnimatorDriver,
        SIGNAL(advanced(QQuickWindow*)), receiver, method, type );
}

#ifndef QT_NO_DEBUG_STREAM

void QskAnimator::debugStatistics( QDebug debug )
{
    if ( qskStatistics )
        qskStatistics->debugStatistics( debug );
}

#endif

#include "QskAnimator.moc"
