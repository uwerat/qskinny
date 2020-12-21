/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_EVENT_H
#define QSK_EVENT_H

#include "QskAspect.h"

#include <qcoreevent.h>
#include <qrect.h>
#include <memory>

class QskGesture;
class QskPopup;
class QQuickWindow;
class QQuickItem;

class QMouseEvent;
class QWheelEvent;
class QHoverEvent;

class QSK_EXPORT QskEvent : public QEvent
{
  public:
    enum Type
    {
        NoEvent = 53800,

        GeometryChange,
        WindowChange,

        /*
            Popups indicate their existence to the owning window
            to allow for priority based stacking rules
         */
        PopupAdded,
        PopupRemoved,

        Gesture,

        Animator,

        MaxEvent = NoEvent + 50
    };

    QskEvent( QskEvent::Type type );

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    virtual QskEvent *clone() const;
#endif

};

class QSK_EXPORT QskGeometryChangeEvent : public QskEvent
{
  public:
    QskGeometryChangeEvent( const QRectF& rect, const QRectF& oldRect );

    inline const QRectF& rect() const { return m_rect; }
    inline const QRectF& oldRect() const { return m_oldRect; }

    bool isResized() const;
    bool isMoved() const;

    QskGeometryChangeEvent* clone() const override;

  private:
    const QRectF m_rect;
    const QRectF m_oldRect;
};

class QSK_EXPORT QskWindowChangeEvent : public QskEvent
{
  public:
    QskWindowChangeEvent( QQuickWindow* oldWindow, QQuickWindow* window );

    inline QQuickWindow* window() const { return m_window; }
    inline QQuickWindow* oldWindow() const { return m_oldWindow; }

    QskWindowChangeEvent* clone() const override;

  private:
    QQuickWindow* const m_oldWindow;
    QQuickWindow* const m_window;
};

class QSK_EXPORT QskPopupEvent : public QskEvent
{
  public:
    QskPopupEvent( Type, QskPopup* );

    inline QskPopup* popup() const { return m_popup; }

    QskPopupEvent* clone() const override;

  private:
    QskPopup* const m_popup;
};

class QSK_EXPORT QskGestureEvent : public QskEvent
{
  public:
    QskGestureEvent( std::shared_ptr< const QskGesture > );

    inline std::shared_ptr< const QskGesture > gesture() const { return m_gesture; }

    QskGestureEvent* clone() const override;

  private:
    std::shared_ptr< const QskGesture > m_gesture;
};

class QSK_EXPORT QskAnimatorEvent : public QskEvent
{
  public:
    enum State
    {
        Started,
        Terminated
    };

    QskAnimatorEvent( QskAspect aspect, State state );

    inline QskAspect aspect() const { return m_aspect; }
    inline State state() const { return m_state; }

    QskAnimatorEvent* clone() const override;

  private:
    const QskAspect m_aspect;
    const State m_state;
};

QSK_EXPORT int qskFocusChainIncrement( const QEvent* );

// some helper to work around Qt version incompatibilities
QSK_EXPORT QPointF qskMouseScenePosition( const QMouseEvent* );
QSK_EXPORT QPointF qskMousePosition( const QMouseEvent* );
QSK_EXPORT QPointF qskWheelPosition( const QWheelEvent* );
QSK_EXPORT QPointF qskHoverPosition( const QHoverEvent* );

#endif
