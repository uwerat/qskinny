/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskEvent.h"
#include "QskGesture.h"

#include <qevent.h>

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

int qskFocusChainIncrement( const QEvent* event )
{
    if ( event && event->type() == QEvent::KeyPress )
    {
        const auto keyEvent = static_cast< const QKeyEvent* >( event );
        if ( !( keyEvent->modifiers() & ( Qt::ControlModifier | Qt::AltModifier ) ) )
        {
            switch ( keyEvent->key() )
            {
                case Qt::Key_Tab:
                    return 1;

                case Qt::Key_Backtab:
                    return -1;
            }
        }
    }

    return 0;
}

QPointF qskMousePosition( const QMouseEvent* event )
{
#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
    return event->position();
#else
    return event->localPos();
#endif
}

QPointF qskMouseScenePosition( const QMouseEvent* event )
{
#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
    return event->scenePosition();
#else
    return event->windowPos();
#endif
}

QPointF qskHoverPosition( const QHoverEvent* event )
{
#if QT_VERSION >= QT_VERSION_CHECK( 6, 0, 0 )
    return event->position();
#else
    return event->posF();
#endif
}

#ifndef QT_NO_WHEELEVENT

QPointF qskWheelPosition( const QWheelEvent* event )
{
    return event->position();
}

#endif

#ifndef QT_NO_WHEELEVENT

qreal qskWheelSteps( const QWheelEvent* event )
{
    const auto angleDelta = event->angleDelta();

    const qreal delta = angleDelta.y() ? angleDelta.y() : angleDelta.x();
    return delta / QWheelEvent::DefaultDeltasPerStep;
}

qreal qskWheelIncrement( const QWheelEvent* event )
{
    /*
        Some controls ( f.e a spin box ) are interpreting the wheel to
        in/decrease a value. For those the physical direction is not relevant
        and we might have to invert the y delta.
     */

    auto angleDelta = event->angleDelta();

    if ( event->inverted() )
        angleDelta.setY( -angleDelta.y() );

    const qreal delta = angleDelta.y() ? angleDelta.y() : angleDelta.x();
    return delta / QWheelEvent::DefaultDeltasPerStep;
}

#endif

bool qskIsStandardKeyInput( const QKeyEvent* event, QKeySequence::StandardKey key )
{
#if 1
    return event->matches( key );
#else
    constexpr auto mask = ~( Qt::KeypadModifier | Qt::GroupSwitchModifier );

    // We should route this call through the skin. TODO
    const auto theme = QGuiApplicationPrivate::platformTheme();
    const auto bindings = theme->keyBindings( ( event->modifiers() | event->key() ) & mask );

    return bindings.contains( QKeySequence(searchkey) );
#endif
}

QskEvent::QskEvent( QskEvent::Type type )
    : QEvent( static_cast< QEvent::Type >( type ) )
{
}

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )

QskEvent* QskEvent::clone() const
{
    return new QskEvent( *this );
}

#endif
// -- QskGeometryChangeEvent

QskGeometryChangeEvent::QskGeometryChangeEvent(
        const QRectF& rect, const QRectF& oldRect )
    : QskEvent( QskEvent::GeometryChange )
    , m_rect( rect )
    , m_oldRect( oldRect )
{
}

QskGeometryChangeEvent* QskGeometryChangeEvent::clone() const
{
    return new QskGeometryChangeEvent( *this );
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
        QQuickWindow* oldWindow, QQuickWindow* window )
    : QskEvent( QskEvent::WindowChange )
    , m_oldWindow( oldWindow )
    , m_window( window )
{
}

QskWindowChangeEvent* QskWindowChangeEvent::clone() const
{
    return new QskWindowChangeEvent( *this );
}

// -- QskPopupEvent

QskPopupEvent::QskPopupEvent( Type type, QskPopup* popup )
    : QskEvent( type )
    , m_popup( popup )
{
}

QskPopupEvent* QskPopupEvent::clone() const
{
    return new QskPopupEvent( *this );
}

// -- QskGestureEvent

QskGestureEvent::QskGestureEvent( std::shared_ptr< const QskGesture > gesture )
    : QskEvent( QskEvent::Gesture )
    , m_gesture( gesture )
{
}

QskGestureEvent* QskGestureEvent::clone() const
{
    return new QskGestureEvent( *this );
}

// -- QskAnimatorEvent

QskAnimatorEvent::QskAnimatorEvent( QskAspect aspect, int index, State state )
    : QskEvent( QskEvent::Animator )
    , m_aspect( aspect )
    , m_index( index )
    , m_state( state )
{
}

QskAnimatorEvent* QskAnimatorEvent::clone() const
{
    return new QskAnimatorEvent( *this );
}
