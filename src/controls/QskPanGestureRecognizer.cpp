/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskPanGestureRecognizer.h"
#include "QskEvent.h"
#include "QskGesture.h"

#include <qline.h>
#include <qmath.h>
#include <qquickitem.h>
#include <qguiapplication.h>
#include <qstylehints.h>

static inline bool qskIsInOrientation(
    const QPointF& from, const QPointF& to, Qt::Orientations orientations )
{
    if ( orientations == ( Qt::Horizontal | Qt::Vertical ) )
        return true;

    const qreal dx = std::fabs( to.x() - from.x() );
    const qreal dy = std::fabs( to.y() - from.y() );

    const qreal ratio = 0.75;

    if ( orientations == Qt::Horizontal )
        return ( dx >= ratio * dy );

    if ( orientations == Qt::Vertical )
        return ( dy >= ratio * dx );

    return false; // should never happen
}

static inline qreal qskDistance(
    const QPointF& from, const QPointF& to, Qt::Orientations orientations )
{
    if ( orientations == Qt::Horizontal )
        return std::fabs( to.x() - from.x() );

    if ( orientations == Qt::Vertical )
        return std::fabs( to.y() - from.y() );

    const qreal dx = to.x() - from.x();
    const qreal dy = to.y() - from.y();

    return qSqrt( dx * dx + dy * dy );
}

static inline qreal qskAngle(
    const QPointF& from, const QPointF& to, Qt::Orientations orientations )
{
    if ( orientations == Qt::Horizontal )
        return ( to.x() >= from.x() ) ? 0.0 : 180.0;

    if ( orientations == Qt::Vertical )
        return ( to.y() >= from.y() ) ? 270.0 : 90.0;

    return QLineF( from, to ).angle();
}

static void qskSendPanGestureEvent(
    QskGestureRecognizer* recognizer, QskGesture::State state,
    qreal velocity, qreal angle, const QPointF& origin,
    const QPointF& lastPosition, const QPointF& position )
{
    auto item = recognizer->targetItem();
    if ( item == nullptr )
        item = recognizer->watchedItem();

    auto gesture = std::make_shared< QskPanGesture >();
    gesture->setState( state );

    gesture->setAngle( angle );
    gesture->setVelocity( velocity );

    gesture->setOrigin( origin );
    gesture->setLastPosition( lastPosition );
    gesture->setPosition( position );

    QskGestureEvent event( gesture );
    QCoreApplication::sendEvent( item, &event );
}

namespace
{
    class VelocityTracker
    {
      public:
        VelocityTracker()
        {
            reset();
        }

        void record( int elapsed, qreal velocity )
        {
            if ( ( velocity != 0.0 ) && ( m_values[ m_pos ].velocity != 0.0 ) )
            {
                if ( ( velocity > 0.0 ) != ( m_values[ m_pos ].velocity > 0.0 ) )
                    reset(); // direction has changed
            }

            m_values[ m_pos ].elapsed = elapsed;
            m_values[ m_pos ].velocity = velocity;

            m_pos = ( m_pos + 1 ) % Count;
        }

        inline void reset()
        {
            for ( auto& v : m_values )
            {
                v.elapsed = -1;
                v.velocity = 0;
            }
            m_pos = 0;
        }

        inline qreal velocity( ulong elapsed ) const
        {
            qreal sum = 0;
            int numVelocities = 0;

            for ( const auto& v : m_values )
            {
                // only swipe events within the last 500 ms will be considered
                if ( v.elapsed > 0 && ( elapsed - v.elapsed ) <= 500 )
                {
                    sum += v.velocity;
                    numVelocities++;
                }
            }

            return ( numVelocities > 0 ) ? ( sum / numVelocities ) : 0.0;
        }

      private:
        int m_pos = 0;
        enum { Count = 3 };

        struct
        {
            int elapsed = 0;
            qreal velocity = 0.0;
        } m_values[ Count ];
    };
}

class QskPanGestureRecognizer::PrivateData
{
  public:
    Qt::Orientations orientations = Qt::Horizontal | Qt::Vertical;

    int minDistance = QGuiApplication::styleHints()->startDragDistance() + 5;

    quint64 timestampVelocity = 0.0; // timestamp of the last mouse event
    qreal angle = 0.0;

    QPointF origin;
    QPointF pos; // position of the last mouse event

    VelocityTracker velocityTracker;
};

QskPanGestureRecognizer::QskPanGestureRecognizer( QObject* parent )
    : QskGestureRecognizer( parent )
    , m_data( new PrivateData() )
{
    setTimeout( 100 ); // value from the platform ???
}

QskPanGestureRecognizer::~QskPanGestureRecognizer()
{
}

void QskPanGestureRecognizer::setOrientations( Qt::Orientations orientations )
{
    m_data->orientations = orientations;
}

Qt::Orientations QskPanGestureRecognizer::orientations() const
{
    return m_data->orientations;
}

void QskPanGestureRecognizer::setMinDistance( int pixels )
{
    m_data->minDistance = qMax( pixels, 0 );
}

int QskPanGestureRecognizer::minDistance() const
{
    return m_data->minDistance;
}

void QskPanGestureRecognizer::processPress( const QPointF& pos, quint64, bool isFinal )
{
    /*
        When nobody was interested in the press we can disable the timeout and let
        the distance of the mouse moves be the only criterion.
     */
    setRejectOnTimeout( !isFinal );

    m_data->origin = m_data->pos = pos;
    m_data->timestampVelocity = timestampStarted();

    m_data->velocityTracker.reset();
}

void QskPanGestureRecognizer::processMove( const QPointF& pos, quint64 timestamp )
{
    const ulong elapsed = timestamp - m_data->timestampVelocity;
    const ulong elapsedTotal = timestamp - timestampStarted();

    const QPointF oldPos = m_data->pos;

    m_data->timestampVelocity = timestamp;
    m_data->pos = pos;

    if ( elapsedTotal > 0 ) // ???
    {
        const qreal dist = qskDistance( oldPos, m_data->pos, m_data->orientations );

        const qreal velocity = dist / ( elapsed / 1000.0 );
        m_data->velocityTracker.record( elapsedTotal, velocity );
    }

    bool started = false;

    if ( state() != QskGestureRecognizer::Accepted )
    {
        const qreal dist = qskDistance(
            m_data->origin, m_data->pos, m_data->orientations );

        if ( ( qAbs( dist ) >= m_data->minDistance ) &&
            qskIsInOrientation( m_data->origin, m_data->pos, m_data->orientations ) )
        {
            accept();
            started = true;
        }
    }

    m_data->angle = qskAngle( oldPos, m_data->pos, m_data->orientations );

    if ( state() == QskGestureRecognizer::Accepted )
    {
        const qreal velocity = m_data->velocityTracker.velocity( elapsedTotal );

        if ( started )
        {
            qskSendPanGestureEvent( this, QskGesture::Started,
                velocity, m_data->angle, m_data->origin, m_data->origin, m_data->pos );
        }
        else
        {
            qskSendPanGestureEvent( this, QskGesture::Updated,
                velocity, m_data->angle, m_data->origin, oldPos, m_data->pos );
        }
    }
}

void QskPanGestureRecognizer::processRelease( const QPointF&, quint64 timestamp )
{
    if ( state() == QskGestureRecognizer::Accepted )
    {
        const ulong elapsedTotal = timestamp - timestampStarted();
        const qreal velocity = m_data->velocityTracker.velocity( elapsedTotal );

        qskSendPanGestureEvent( this, QskGesture::Finished,
            velocity, m_data->angle, m_data->origin, m_data->pos, m_data->pos );
    }
}
