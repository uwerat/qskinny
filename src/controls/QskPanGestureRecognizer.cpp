#include "QskPanGestureRecognizer.h"
#include "QskEvent.h"
#include "QskGesture.h"

#include <qcoreapplication.h>
#include <qline.h>
#include <qmath.h>
#include <qquickitem.h>

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
    QQuickItem* item, QskGesture::State state, qreal velocity, qreal angle,
    const QPointF& origin, const QPointF& lastPosition, const QPointF& position )
{
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
    PrivateData()
        : orientations( Qt::Horizontal | Qt::Vertical )
        , minDistance( 15 )
        , timestamp( 0.0 )
        , angle( 0.0 )
    {
    }

    Qt::Orientations orientations;

    int minDistance;

    ulong timestamp; // timestamp of the last mouse event
    qreal angle;

    QPointF origin;
    QPointF pos; // position of the last mouse event

    VelocityTracker velocityTracker;
};

QskPanGestureRecognizer::QskPanGestureRecognizer()
    : m_data( new PrivateData() )
{
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

void QskPanGestureRecognizer::pressEvent( const QMouseEvent* event )
{
    m_data->origin = m_data->pos = qskMousePosition( event );
    m_data->timestamp = timestamp();

    m_data->velocityTracker.reset();
}

void QskPanGestureRecognizer::moveEvent( const QMouseEvent* event )
{
    const ulong elapsed = event->timestamp() - m_data->timestamp;
    const ulong elapsedTotal = event->timestamp() - timestamp();

    const QPointF oldPos = m_data->pos;

    m_data->timestamp = event->timestamp();
    m_data->pos = qskMousePosition( event );

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
            qskSendPanGestureEvent( watchedItem(), QskGesture::Started,
                velocity, m_data->angle, m_data->origin, m_data->origin, m_data->pos );
        }
        else
        {
            qskSendPanGestureEvent( watchedItem(), QskGesture::Updated,
                velocity, m_data->angle, m_data->origin, oldPos, m_data->pos );
        }
    }
}

void QskPanGestureRecognizer::releaseEvent( const QMouseEvent* event )
{
    if ( state() == QskGestureRecognizer::Accepted )
    {
        const ulong elapsedTotal = event->timestamp() - timestamp();
        const qreal velocity = m_data->velocityTracker.velocity( elapsedTotal );

        qskSendPanGestureEvent( watchedItem(), QskGesture::Finished,
            velocity, m_data->angle, m_data->origin, m_data->pos, m_data->pos );
    }
}
