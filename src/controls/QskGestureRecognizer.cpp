/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskGestureRecognizer.h"
#include "QskEvent.h"
#include "QskQuick.h"
#include "QskInternalMacros.h"

#include <qcoreapplication.h>
#include <qquickitem.h>
#include <qquickwindow.h>
#include <qvector.h>

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    QSK_QT_PRIVATE_BEGIN
        #include <private/qquickwindow_p.h>
    QSK_QT_PRIVATE_END
#endif

static QMouseEvent* qskClonedMouseEvent( const QMouseEvent* event )
{
    QMouseEvent* clonedEvent;

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    clonedEvent = QQuickWindowPrivate::cloneMouseEvent(
        const_cast< QMouseEvent* >( event ), nullptr );
#else
    clonedEvent = event->clone();
#endif
    clonedEvent->setAccepted( false );

    return clonedEvent;
}

class QskGestureRecognizer::PrivateData
{
  public:
    void startTimer( QskGestureRecognizer* recognizer )
    {
        if ( timeoutId >= 0 )
        {
            // warning
        }

        if ( timeout <= 0 )
        {
            // warning
        }

        timeoutId = recognizer->startTimer( timeout );
    }

    void stopTimer( QskGestureRecognizer* recognizer )
    {
        if ( timeoutId >= 0 )
        {
            recognizer->killTimer( timeoutId );
            timeoutId = -1;
        }
    }

    inline Qt::MouseButtons effectiveMouseButtons() const
    {
        if ( buttons != Qt::NoButton )
            return buttons;

        return watchedItem->acceptedMouseButtons();
    }

    QPointer< QQuickItem > watchedItem = nullptr;
    QPointer< QQuickItem > targetItem = nullptr;

    QVector< QMouseEvent* > pendingEvents;

    quint64 timestampStarted = 0;
    quint64 timestampProcessed = 0;

    int timeoutId = -1;
    int timeout = -1; // ms

    Qt::MouseButtons buttons = Qt::NoButton;

    unsigned char state = QskGestureRecognizer::Idle;
    bool rejectOnTimeout = true;
    bool expired = false;
};

QskGestureRecognizer::QskGestureRecognizer( QObject* parent )
    : QObject( parent )
    , m_data( new PrivateData() )
{
}

QskGestureRecognizer::~QskGestureRecognizer()
{
    qDeleteAll( m_data->pendingEvents );
}

void QskGestureRecognizer::setWatchedItem( QQuickItem* item )
{
    if ( m_data->watchedItem )
    {
        m_data->watchedItem->removeEventFilter( this );
        reset();
    }

    m_data->watchedItem = item;

    if ( m_data->watchedItem )
        m_data->watchedItem->installEventFilter( this );

    /*
        // doing those here: ???
        m_data->watchedItem->setFiltersChildMouseEvents();
        m_data->watchedItem->setAcceptedMouseButtons();
     */
}

QQuickItem* QskGestureRecognizer::watchedItem() const
{
    return m_data->watchedItem;
}

void QskGestureRecognizer::setTargetItem( QQuickItem* item )
{
    m_data->targetItem = item;
}

QQuickItem* QskGestureRecognizer::targetItem() const
{
    return m_data->targetItem;
}

void QskGestureRecognizer::setAcceptedMouseButtons( Qt::MouseButtons buttons )
{
    m_data->buttons = buttons;
}

Qt::MouseButtons QskGestureRecognizer::acceptedMouseButtons() const
{
    return m_data->buttons;
}

bool QskGestureRecognizer::isAcceptedPos( const QPointF& pos ) const
{
    return m_data->watchedItem && m_data->watchedItem->contains( pos );
}

void QskGestureRecognizer::setRejectOnTimeout( bool on )
{
    m_data->rejectOnTimeout = on;
}

bool QskGestureRecognizer::rejectOnTimeout() const
{
    return m_data->rejectOnTimeout;
}

void QskGestureRecognizer::setTimeout( int ms )
{
    m_data->timeout = ms;
}

int QskGestureRecognizer::timeout() const
{
    return m_data->timeout;
}

void QskGestureRecognizer::timerEvent( QTimerEvent* event )
{
    if ( event->timerId() == m_data->timeoutId )
    {
        m_data->stopTimer( this );

        if ( m_data->rejectOnTimeout )
        {
            reject();
            m_data->expired = true;
        }

        return;
    }

    Inherited::timerEvent( event );
}

quint64 QskGestureRecognizer::timestampStarted() const
{
    return m_data->timestampStarted;
}

void QskGestureRecognizer::setState( State state )
{
    if ( state != m_data->state )
    {
        const auto oldState = static_cast< State >( m_data->state );
        m_data->state = state;

        Q_EMIT stateChanged( oldState, state );
    }
}

QskGestureRecognizer::State QskGestureRecognizer::state() const
{
    return static_cast< QskGestureRecognizer::State >( m_data->state );
}

bool QskGestureRecognizer::eventFilter( QObject* object, QEvent* event)
{
    auto& watchedItem = m_data->watchedItem;

    if ( ( object == watchedItem ) &&
        ( static_cast< int >( event->type() ) == QskEvent::GestureFilter ) )
    {
        if ( !watchedItem->isEnabled() || !watchedItem->isVisible()
            || watchedItem->window() == nullptr )
        {
            reset();
            return false;
        }

        auto ev = static_cast< QskGestureFilterEvent* >( event );
        ev->setMaybeGesture( maybeGesture( ev->item(), ev->event() ) );

        return ev->maybeGesture();
    }

    return false;
}

bool QskGestureRecognizer::maybeGesture(
    const QQuickItem* item, const QEvent* event )
{
    switch( static_cast< int >( event->type() ) )
    {
        case QEvent::UngrabMouse:
        {
            if ( m_data->state != Idle )
            {
                // someone took our grab away, we have to give up
                reset();
                return true;
            }

            return false;
        }
        case QEvent::MouseButtonPress:
        {
            if ( state() != Idle )
            {
                qWarning() << "QskGestureRecognizer: pressed, while not being idle";
                return false;
            }

            const auto mouseEvent = static_cast< const QMouseEvent* >( event );

            if ( mouseEvent->timestamp() > m_data->timestampProcessed )
            {
                m_data->timestampProcessed = mouseEvent->timestamp();
            }
            else
            {
                /*
                    A  mouse event might appear several times:

                        - we ran into a timeout and reposted the events

                        - we rejected because the event sequence does
                          not match the acceptance criterions and reposted
                          the events

                        - another gesture recognizer for a child item
                          has reposted the events because of the reasons above

                    For most situations we can simply skip processing already
                    processed events with the exception of a timeout. Here we might
                    have to retry without timeout, when none of the items was
                    accepting the events. This specific situation is indicated by
                    item set to null.
                 */

                if ( item || !m_data->expired )
                    return false;
            }

            return processMouseEvent( item, mouseEvent );
        }
        case QEvent::MouseMove:
        case QEvent::MouseButtonRelease:
        {
            if ( state() <= Idle )
                return false;

            const auto mouseEvent = static_cast< const QMouseEvent* >( event );
            return processMouseEvent( item, mouseEvent );
        }
    }

    return false;
}

bool QskGestureRecognizer::processMouseEvent(
    const QQuickItem* item, const QMouseEvent* event )
{
    auto& watchedItem = m_data->watchedItem;

    const auto pos = watchedItem->mapFromScene( qskMouseScenePosition( event ) );
    const auto timestamp = event->timestamp();

    if ( event->type() == QEvent::MouseButtonPress )
    {
        if ( !isAcceptedPos( pos ) )
            return false;

        if ( m_data->state != Idle )
        {
            // should not happen, when using the recognizer correctly
            qWarning() << "QskGestureRecognizer: pressed, while not being idle";
            abort();
            return false;
        }

        if ( !( m_data->effectiveMouseButtons() & event->button() ) )
            return false;

        /*
            We try to grab the mouse for watchedItem and indicate, that we want
            to keep it. Then all mouse events should end up at watchedItem.
         */
        if ( !qskGrabMouse( watchedItem ) )
            return false;

        m_data->timestampStarted = timestamp;

        if ( m_data->timeout != 0 )
        {
            /*
                We need to be able to replay the press in case we later find
                out, that we don't want to handle the mouse event sequence,
             */

            m_data->stopTimer( this );

            if ( m_data->timeout > 0 )
                m_data->startTimer( this );

            setState( Pending );
        }
        else
        {
            accept();
        }
    }

    if ( m_data->state <= Idle )
        return false;

    if ( m_data->state == Pending )
        m_data->pendingEvents += qskClonedMouseEvent( event );

    switch( static_cast< int >( event->type() ) )
    {
        case QEvent::MouseButtonPress:
            processPress( pos, timestamp, item == nullptr );
            break;

        case QEvent::MouseMove:
            processMove( pos, timestamp );
            break;

        case QEvent::MouseButtonRelease:
        {
            if ( m_data->state == Pending )
            {
                reject();
            }
            else
            {
                processRelease( pos, timestamp );
                reset();
            }
            break;
        }
    }

    return true;
}

void QskGestureRecognizer::processPress(
    const QPointF& pos, quint64 timestamp, bool isFinal )
{
    Q_UNUSED( pos );
    Q_UNUSED( timestamp );
    Q_UNUSED( isFinal );
}

void QskGestureRecognizer::processMove( const QPointF& pos, quint64 timestamp )
{
    Q_UNUSED( pos );
    Q_UNUSED( timestamp );
}

void QskGestureRecognizer::processRelease( const QPointF& pos, quint64 timestamp )
{
    Q_UNUSED( pos );
    Q_UNUSED( timestamp );
}

void QskGestureRecognizer::accept()
{
    m_data->stopTimer( this );

    qDeleteAll( m_data->pendingEvents );
    m_data->pendingEvents.clear();

    setState( Accepted );
}

void QskGestureRecognizer::reject()
{
    /*
        Moving the events to a local buffer, so that we can clear
        m_data->pendingEvents before replaying them.
     */
    const auto events = m_data->pendingEvents;
    m_data->pendingEvents.clear();

    reset();

    /*
        Not 100% sure if we should send or post the events

        Posting the events adds an extra round trip but we avoid
        recursive event handler calls, that might not be expected
        from the implementation of a control.
     */

    if ( !events.isEmpty() )
    {
        if ( auto watchedItem = m_data->watchedItem )
        {
            if ( const auto window = watchedItem->window() )
            {
                for ( auto event : events )
                    QCoreApplication::postEvent( window, event );
            }
        }

#if 0
        // when using QCoreApplication::sendEvent above
        qDeleteAll( events );
#endif
    }
}

void QskGestureRecognizer::abort()
{
    reset();
}

void QskGestureRecognizer::reset()
{
    m_data->stopTimer( this );
    qskUngrabMouse( m_data->watchedItem );

    qDeleteAll( m_data->pendingEvents );
    m_data->pendingEvents.clear();

    m_data->timestampStarted = 0;
    m_data->expired = false;

    setState( Idle );
}

#include "moc_QskGestureRecognizer.cpp"
