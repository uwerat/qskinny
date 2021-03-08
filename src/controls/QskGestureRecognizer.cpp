#include "QskGestureRecognizer.h"
#include "QskEvent.h"
#include "QskQuick.h"

#include <qbasictimer.h>
#include <qcoreapplication.h>
#include <qcoreevent.h>
#include <qquickitem.h>
#include <qquickwindow.h>
#include <qscopedpointer.h>
#include <qvector.h>

QSK_QT_PRIVATE_BEGIN
#include <private/qquickwindow_p.h>
QSK_QT_PRIVATE_END

static QMouseEvent* qskClonedMouseEventAt(
    const QMouseEvent* event, QPointF* localPos )
{
#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )

    auto clonedEvent = QQuickWindowPrivate::cloneMouseEvent(
        const_cast< QMouseEvent* >( event ), localPos );

#else
    auto clonedEvent = event->clone();

    auto& point = QMutableEventPoint::from( clonedEvent->point( 0 ) );
    point.detach();
    point.setTimestamp( event->timestamp() );

    if ( localPos )
        point.setPosition( *localPos );
#endif

    return clonedEvent;
}

static inline QMouseEvent* qskClonedMouseEvent(
    const QMouseEvent* mouseEvent, const QQuickItem* item = nullptr )
{
    QMouseEvent* clonedEvent;
    auto event = const_cast< QMouseEvent* >( mouseEvent );

    if ( item )
    {
        auto localPos = item->mapFromScene( qskMouseScenePosition( event ) );
        clonedEvent = qskClonedMouseEventAt( event, &localPos );
    }
    else
    {
        clonedEvent = qskClonedMouseEventAt( event, nullptr );
    }

    clonedEvent->setAccepted( false );
    return clonedEvent;
}

namespace
{
    /*
        As we don't want QskGestureRecognizer being a QObject
        we need some extra timers - usually one per screen.
     */

    class Timer final : public QObject
    {
      public:
        void start( int ms, QskGestureRecognizer* recognizer )
        {
            if ( m_timer.isActive() )
                qWarning() << "QskGestureRecognizer: resetting an active timer";

            m_recognizer = recognizer;
            m_timer.start( ms, this );
        }

        void stop()
        {
            m_timer.stop();
            m_recognizer = nullptr;
        }

        const QskGestureRecognizer* recognizer() const
        {
            return m_recognizer;
        }

      protected:
        void timerEvent( QTimerEvent* ) override
        {
            m_timer.stop();

            if ( m_recognizer )
            {
                auto recognizer = m_recognizer;
                m_recognizer = nullptr;

                recognizer->reject();
            }
        }

        QBasicTimer m_timer;
        QskGestureRecognizer* m_recognizer = nullptr;
    };

    class TimerTable
    {
      public:
        ~TimerTable()
        {
            qDeleteAll( m_table );
        }

        void startTimer( int ms, QskGestureRecognizer* recognizer )
        {
            Timer* timer = nullptr;

            for ( auto t : qskAsConst( m_table ) )
            {
                if ( t->recognizer() == nullptr ||
                    t->recognizer() == recognizer )
                {
                    timer = t;
                    break;
                }
            }

            if ( timer == nullptr )
            {
                timer = new Timer();
                m_table += timer;
            }

            timer->start( ms, recognizer );
        }

        void stopTimer( const QskGestureRecognizer* recognizer )
        {
            for ( auto timer : qskAsConst( m_table ) )
            {
                if ( timer->recognizer() == recognizer )
                {
                    // we keep the timer to be used later again
                    timer->stop();
                    return;
                }
            }
        }

      private:
        /*
            Usually we have not more than one entry.
            Only when having more than one screen we
            might have mouse events to be processed
            simultaneously.
         */
        QVector< Timer* > m_table;
    };

    class PendingEvents : public QVector< QMouseEvent* >
    {
      public:
        ~PendingEvents()
        {
            qDeleteAll( *this );
        }

        void reset()
        {
            qDeleteAll( *this );
            clear();
        }
    };
}

Q_GLOBAL_STATIC( TimerTable, qskTimerTable )

class QskGestureRecognizer::PrivateData
{
  public:
    PrivateData()
        : watchedItem( nullptr )
        , timestamp( 0 )
        , timestampProcessed( 0 )
        , timeout( -1 )
        , buttons( Qt::NoButton )
        , state( QskGestureRecognizer::Idle )
        , isReplayingEvents( false )
    {
    }

    QQuickItem* watchedItem;

    PendingEvents pendingEvents;
    ulong timestamp;
    ulong timestampProcessed;

    int timeout; // ms

    Qt::MouseButtons buttons;

    int state : 4;
    bool isReplayingEvents : 1; // not exception safe !!!
};

QskGestureRecognizer::QskGestureRecognizer()
    : m_data( new PrivateData() )
{
}

QskGestureRecognizer::~QskGestureRecognizer()
{
    qskTimerTable->stopTimer( this );
}

void QskGestureRecognizer::setWatchedItem( QQuickItem* item )
{
    if ( m_data->watchedItem )
        reset();

    m_data->watchedItem = item;
}

QQuickItem* QskGestureRecognizer::watchedItem() const
{
    return m_data->watchedItem;
}

void QskGestureRecognizer::setAcceptedMouseButtons( Qt::MouseButtons buttons )
{
    m_data->buttons = buttons;
}

Qt::MouseButtons QskGestureRecognizer::acceptedMouseButtons() const
{
    return m_data->buttons;
}

void QskGestureRecognizer::setTimeout( int ms )
{
    m_data->timeout = ms;
}

int QskGestureRecognizer::timeout() const
{
    return m_data->timeout;
}

ulong QskGestureRecognizer::timestamp() const
{
    return m_data->timestamp;
}

bool QskGestureRecognizer::hasProcessedBefore( const QMouseEvent* event ) const
{
    return event && ( event->timestamp() <= m_data->timestampProcessed );
}

bool QskGestureRecognizer::isReplaying() const
{
    return m_data->isReplayingEvents;
}

void QskGestureRecognizer::setState( State state )
{
    if ( state != m_data->state )
    {
        const State oldState = static_cast< QskGestureRecognizer::State >( m_data->state );
        m_data->state = state;

        stateChanged( oldState, state );
    }
}

QskGestureRecognizer::State QskGestureRecognizer::state() const
{
    return static_cast< QskGestureRecognizer::State >( m_data->state );
}

bool QskGestureRecognizer::processEvent(
    QQuickItem* item, QEvent* event, bool blockReplayedEvents )
{
    if ( m_data->isReplayingEvents && blockReplayedEvents )
    {
        /*
            This one is a replayed event after we had decided
            that this interaction is to be ignored
         */
        return false;
    }

    auto& watchedItem = m_data->watchedItem;

    if ( watchedItem == nullptr || !watchedItem->isEnabled() ||
        !watchedItem->isVisible() || watchedItem->window() == nullptr )
    {
        reset();
        return false;
    }

    QScopedPointer< QMouseEvent > clonedPress;

    if ( event->type() == QEvent::MouseButtonPress )
    {
        if ( m_data->state != Idle )
        {
            // should not happen, when using the recognizer correctly
            qWarning() << "QskGestureRecognizer: pressed, while not being idle";
            abort();
            return false;
        }

        Qt::MouseButtons buttons = m_data->buttons;
        if ( buttons == Qt::NoButton )
            buttons = watchedItem->acceptedMouseButtons();

        auto mouseEvent = static_cast< QMouseEvent* >( event );
        if ( !( buttons & mouseEvent->button() ) )
            return false;

        /*
            We try to grab the mouse for watchedItem and indicate, that we want
            to keep it. Then all mouse events should end up at watchedItem.
         */
        if ( !qskGrabMouse( watchedItem ) )
            return false;

        m_data->timestamp = mouseEvent->timestamp();

        if ( item != watchedItem )
        {
            /*
                The first press happens before having the mouse grab and might
                have been for a child of watchedItem. Then we create a clone
                of the event with positions translated into the coordinate system
                of watchedItem.
             */

            clonedPress.reset( qskClonedMouseEvent( mouseEvent, watchedItem ) );

            item = watchedItem;
            event = clonedPress.data();
        }

        if ( m_data->timeout != 0 )
        {
            /*
                We need to be able to replay the press in case we later find
                out, that we don't want to handle the mouse event sequence,
             */

            if ( m_data->timeout > 0 )
                qskTimerTable->startTimer( m_data->timeout, this );

            setState( Pending );
        }
        else
        {
            setState( Accepted );
        }
    }

    if ( ( item == watchedItem ) && ( m_data->state > Idle ) )
    {
        switch ( event->type() )
        {
            case QEvent::MouseButtonPress:
            {
                auto mouseEvent = static_cast< QMouseEvent* >( event );
                m_data->pendingEvents += qskClonedMouseEvent( mouseEvent );

                pressEvent( mouseEvent );
                return true;
            }

            case QEvent::MouseMove:
            {
                auto mouseEvent = static_cast< QMouseEvent* >( event );
                m_data->pendingEvents += qskClonedMouseEvent( mouseEvent );

                moveEvent( mouseEvent );
                return true;
            }

            case QEvent::MouseButtonRelease:
            {
                auto mouseEvent = static_cast< QMouseEvent* >( event );
                m_data->pendingEvents += qskClonedMouseEvent( mouseEvent );

                if ( m_data->state == Pending )
                {
                    reject();
                }
                else
                {
                    releaseEvent( mouseEvent );
                    reset();
                }

                return true;
            }

            case QEvent::UngrabMouse:
            {
                // someone took away our grab, we have to give up
                reset();
                break;
            }
            default:
                break;
        }
    }

    return false;
}

void QskGestureRecognizer::pressEvent( const QMouseEvent* )
{
}

void QskGestureRecognizer::moveEvent( const QMouseEvent* )
{
}

void QskGestureRecognizer::releaseEvent( const QMouseEvent* )
{
}

void QskGestureRecognizer::stateChanged( State from, State to )
{
    Q_UNUSED( from )
    Q_UNUSED( to )
}

void QskGestureRecognizer::accept()
{
    qskTimerTable->stopTimer( this );
    m_data->pendingEvents.reset();

    setState( Accepted );
}

void QskGestureRecognizer::reject()
{
    const auto events = m_data->pendingEvents;
    m_data->pendingEvents.clear();

    reset();

    auto watchedItem = m_data->watchedItem;
    if ( watchedItem == nullptr )
        return;

    const auto window = watchedItem->window();
    if ( window == nullptr )
        return;

    m_data->isReplayingEvents = true;

    qskUngrabMouse( watchedItem );

    if ( !events.isEmpty() &&
        ( events[ 0 ]->type() == QEvent::MouseButtonPress ) )
    {
        /*
            In a situation of several recognizers ( f.e a vertical
            scroll view inside a horizontal swipe view ), we might receive
            cloned events from another recognizer.
            To avoid to process them twice we store the most recent timestamp
            of the cloned events we have already processed, but reposted.
         */

        m_data->timestampProcessed = events.last()->timestamp();

        for ( auto event : events )
            QCoreApplication::sendEvent( window, event );
    }

    m_data->isReplayingEvents = false;
}

void QskGestureRecognizer::abort()
{
    reset();
}

void QskGestureRecognizer::reset()
{
    qskTimerTable->stopTimer( this );

    qskUngrabMouse( m_data->watchedItem );

    m_data->pendingEvents.reset();
    m_data->timestamp = 0;

    setState( Idle );
}
