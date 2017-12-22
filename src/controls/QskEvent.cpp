#include "QskEvent.h"
#include "QskGesture.h"

static void qskRegisterEventTypes()
{
    // We don't ask QEvent::registerEventType for unique ids as it prevents
    // using them in switch/case statements. To avoid conflicts with other
    // applications we at least try to reserve as soon as possible, so that
    // applications behaving better than us does not get them.

    for ( int i = QskEvent::NoEvent; i <= QskEvent::MaxEvent; i++ )
    {
        const int id = QEvent::registerEventType( i );
        Q_ASSERT( id == i );
    }
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterEventTypes )

QskEvent::QskEvent( QskEvent::Type type ):
    QEvent( static_cast< QEvent::Type >( type ) )
{
}

// -- QskGeometryChangeEvent

QskGeometryChangeEvent::QskGeometryChangeEvent(
        const QRectF& rect, const QRectF& oldRect ):
    QskEvent( QskEvent::GeometryChange ),
    m_rect( rect ),
    m_oldRect( oldRect )
{
}

bool QskGeometryChangeEvent::isResized() const
{
    return ( m_rect.width() != m_oldRect.width() ) ||
        ( m_rect.height() != m_oldRect.height() );
}

bool QskGeometryChangeEvent::isMoved() const
{
    return ( m_rect.x() != m_oldRect.x() ) ||
        ( m_rect.y() != m_oldRect.y() );
}

// -- QskWindowChangeEvent

QskWindowChangeEvent::QskWindowChangeEvent(
        QQuickWindow* oldWindow, QQuickWindow* window ):
    QskEvent( QskEvent::WindowChange ),
    m_oldWindow( oldWindow ),
    m_window( window )
{
}

// -- QskGestureEvent

QskGestureEvent::QskGestureEvent( const QskGesture* gesture, bool ownedByEvent ):
    QskEvent( QskEvent::Gesture ),
    m_gesture( gesture ),
    m_gestureOwnedByEvent( ownedByEvent )
{
}

QskGestureEvent::~QskGestureEvent()
{
    if ( m_gestureOwnedByEvent )
        delete m_gesture;
}

// -- QskAnimatorEvent

QskAnimatorEvent::QskAnimatorEvent( QskAspect::Aspect aspect, State state ):
    QskEvent( QskEvent::Animator ),
    m_aspect( aspect ),
    m_state( state )
{
}

QskAnimatorEvent::~QskAnimatorEvent()
{
}
