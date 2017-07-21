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
}

class QskGestureRecognizer::PrivateData
{
public:
    PrivateData():
        watchedItem( nullptr ),
        pendingPress( nullptr ),
        timestamp( 0 ),
        timeout( -1 ),
        buttons( Qt::NoButton ),
        state( QskGestureRecognizer::Idle ),
        isReplayingEvents( false )
    {
    }

    ~PrivateData()
    {
        delete pendingPress;
    }

    QQuickItem* watchedItem;

    QMouseEvent* pendingPress;
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

bool QskGestureRecognizer::processEvent( QQuickItem* item, QEvent* event )
{
    if ( m_data->isReplayingEvents )
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

            m_data->pendingPress = qskClonedMouseEvent( mouseEvent );

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
                pressEvent( static_cast< QMouseEvent* >( event ) );
                return true;
            }

            case QEvent::MouseMove:
            {
                moveEvent( static_cast< QMouseEvent* >( event ) );
                return true;
            }

            case QEvent::MouseButtonRelease:
            {
                auto mouseEvent = static_cast< QMouseEvent* >( event );

                if ( m_data->state == Pending )
                {
                    reject(); // sending the pending press

                    auto mouseGrabber = watchedItem->window()->mouseGrabberItem();
                    if ( mouseGrabber && ( mouseGrabber != watchedItem ) )
                    {
                        /*
                            After resending the initial press someone else
                            might be interested in this sequence. Then we
                            also have to resend the release event, being translated
                            into the coordinate system of the new grabber.
                         */

                        QScopedPointer< QMouseEvent > clonedRelease(
                            qskClonedMouseEvent( mouseEvent, mouseGrabber ) );

                        m_data->isReplayingEvents = true;
                        QCoreApplication::sendEvent(
                            watchedItem->window(), clonedRelease.data() );
                        m_data->isReplayingEvents = false;
                    }
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

    delete m_data->pendingPress;
    m_data->pendingPress = nullptr;

    setState( Accepted );
}

void QskGestureRecognizer::reject()
{
    QScopedPointer< QMouseEvent > mousePress( m_data->pendingPress );
    m_data->pendingPress = nullptr;

    reset();

    if ( mousePress.data() )
    {
        const auto window = m_data->watchedItem->window();
        if ( window->mouseGrabberItem() == m_data->watchedItem )
            m_data->watchedItem->ungrabMouse();

        m_data->isReplayingEvents = true;
        QCoreApplication::sendEvent( window, mousePress.data() );
        m_data->isReplayingEvents = false;
    }
}

void QskGestureRecognizer::abort()
{
    reset();
}

void QskGestureRecognizer::reset()
{
    Timer::instance()->stop( this );
    m_data->watchedItem->setKeepMouseGrab( false );

    delete m_data->pendingPress;
    m_data->pendingPress = nullptr;
    m_data->timestamp = 0;

    setState( Idle );
}
