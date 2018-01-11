#include "QskGestureRecognizer.h"

#include <QQuickWindow>
#include <QQuickItem>
#include <QCoreApplication>
#include <QBasicTimer>
#include <QScopedPointer>
#include <QMouseEvent>

QSK_QT_PRIVATE_BEGIN
#include <private/qquickwindow_p.h>
QSK_QT_PRIVATE_END

static inline QMouseEvent* qskClonedMouseEvent(
    const QMouseEvent* mouseEvent, const QQuickItem* item = nullptr )
{
    QMouseEvent* clonedEvent;
    QMouseEvent* event = const_cast< QMouseEvent* >( mouseEvent );

    if ( item )
    {
        QPointF localPos = item->mapFromScene( event->windowPos() );
        clonedEvent = QQuickWindowPrivate::cloneMouseEvent( event, &localPos );
    }
    else
    {
        clonedEvent = QQuickWindowPrivate::cloneMouseEvent( event, nullptr );
    }

    clonedEvent->setAccepted( false );
    return clonedEvent;
}

namespace
{
    // again we try to avoid creating unnecessary QObjects by
    // using the same timer Object for all gesture recognizers

    class Timer final : public QObject
    {
    public:
        static Timer* instance()
        {
            // ther should be only one mouse being pressed at the same time
            static Timer timer;
            return &timer;
        }

        void start( int ms, QskGestureRecognizer* recognizer )
        {
            if ( m_recognizer && m_recognizer != recognizer )
            {
                qWarning() << "QskGestureRecognizer: running more than one recognizer at the same time";
                m_recognizer->reject(); // should never happen
            }

            m_recognizer = recognizer;
            m_timer.start( ms, this );
        }

        void stop( const QskGestureRecognizer* recognizer )
        {
            if ( recognizer == m_recognizer )
            {
                m_timer.stop();
                m_recognizer = nullptr;
            }
        }

    protected:
        virtual void timerEvent( QTimerEvent* ) override final
        {
            m_timer.stop();

            if ( m_recognizer )
            {
                m_recognizer->reject();
                m_recognizer = nullptr;
            }
        }

        QBasicTimer m_timer;
        QskGestureRecognizer* m_recognizer;
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

class QskGestureRecognizer::PrivateData
{
public:
    PrivateData():
        watchedItem( nullptr ),
        timestamp( 0 ),
        timeout( -1 ),
        buttons( Qt::NoButton ),
        state( QskGestureRecognizer::Idle ),
        isReplayingEvents( false )
    {
    }

    QQuickItem* watchedItem;

    PendingEvents pendingEvents;
    ulong timestamp;

    int timeout; // ms

    Qt::MouseButtons buttons;

    int state : 4;
    bool isReplayingEvents : 1; // not exception safe !!!
};

QskGestureRecognizer::QskGestureRecognizer():
    m_data( new PrivateData() )
{
}

QskGestureRecognizer::~QskGestureRecognizer()
{
    Timer::instance()->stop( this );
}

void QskGestureRecognizer::setWatchedItem( QQuickItem* item )
{
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

    if ( watchedItem == nullptr || !watchedItem->isEnabled()
         || !watchedItem->isVisible() || watchedItem->window() == nullptr )
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
            return false;
        }

        auto mouseGrabber = watchedItem->window()->mouseGrabberItem();
        if ( mouseGrabber && ( mouseGrabber != watchedItem ) )
        {
            if ( mouseGrabber->keepMouseGrab() || mouseGrabber->keepTouchGrab() )
            {
                /*
                    Another child has grabbed mouse/touch and is not willing to
                    be intercepted: we respect this.
                 */

                return false;
            }
        }

        Qt::MouseButtons buttons = m_data->buttons;
        if ( buttons == Qt::NoButton )
            buttons = watchedItem->acceptedMouseButtons();

        auto mouseEvent = static_cast< QMouseEvent* >( event );
        if ( !( buttons & mouseEvent->button() ) )
            return false;

        /*
            We grab the mouse for watchedItem and indicate, that we want
            to keep it. From now on all mouse events should end up at watchedItem.
         */
        watchedItem->grabMouse();
        watchedItem->setKeepMouseGrab( true );

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
                Timer::instance()->start( m_data->timeout, this );

            setState( Pending );
        }
        else
        {
            setState( Accepted );
        }
    }

    if ( ( item == watchedItem ) && ( m_data->state > Idle ) )
    {
        switch( event->type() )
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
    Timer::instance()->stop( this );
    m_data->pendingEvents.reset();

    setState( Accepted );
}

void QskGestureRecognizer::reject()
{
    const auto events = m_data->pendingEvents;
    m_data->pendingEvents.clear();

    reset();

    m_data->isReplayingEvents = true;

    const auto window = m_data->watchedItem->window();

    if ( window->mouseGrabberItem() == m_data->watchedItem )
        m_data->watchedItem->ungrabMouse();

    if ( !events.isEmpty() && events[0]->type() == QEvent::MouseButtonPress )
    {
        QCoreApplication::sendEvent( window, events[0] );

        /*
            After resending the initial press someone else
            might be interested in this sequence.
         */

        if ( window->mouseGrabberItem() )
        {
            for ( int i = 1; i < events.size(); i++ )
                QCoreApplication::sendEvent( window, events[i] );
        }
    }

    m_data->isReplayingEvents = false;
}

void QskGestureRecognizer::abort()
{
    reset();
}

void QskGestureRecognizer::reset()
{
    Timer::instance()->stop( this );
    m_data->watchedItem->setKeepMouseGrab( false );
    m_data->pendingEvents.reset();

    m_data->timestamp = 0;

    setState( Idle );
}
