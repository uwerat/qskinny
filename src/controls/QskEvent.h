/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_EVENT_H
#define QSK_EVENT_H

#include "QskAspect.h"

#include <qcoreevent.h>
#include <qrect.h>

class QskGesture;
class QskPopup;
class QQuickWindow;
class QQuickItem;
class QMouseEvent;

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
};

class QSK_EXPORT QskGeometryChangeEvent : public QskEvent
{
  public:
    QskGeometryChangeEvent( const QRectF& rect, const QRectF& oldRect );

    inline const QRectF& rect() const { return m_rect; }
    inline const QRectF& oldRect() const { return m_oldRect; }

    bool isResized() const;
    bool isMoved() const;

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

  private:
    QQuickWindow* const m_oldWindow;
    QQuickWindow* const m_window;
};

class QSK_EXPORT QskPopupEvent : public QskEvent
{
  public:
    QskPopupEvent( Type, QskPopup* );

    inline QskPopup* popup() const { return m_popup; }

  private:
    QskPopup* const m_popup;
};

class QSK_EXPORT QskGestureEvent : public QskEvent
{
  public:
    QskGestureEvent( const QskGesture* gesture, bool ownedByEvent = true );
    ~QskGestureEvent() override;

    inline const QskGesture* gesture() const { return m_gesture; }
    inline bool isGestureOwnedByEvent() const { return m_gestureOwnedByEvent; }

  private:
    const QskGesture* m_gesture;
    bool m_gestureOwnedByEvent;
};

class QSK_EXPORT QskAnimatorEvent : public QskEvent
{
  public:
    enum State
    {
        Started,
        Terminated
    };

    QskAnimatorEvent( QskAspect::Aspect aspect, State state );
    ~QskAnimatorEvent() override;

    inline QskAspect::Aspect aspect() const { return m_aspect; }
    inline State state() const { return m_state; }

  private:
    const QskAspect::Aspect m_aspect;
    const State m_state;
};

QSK_EXPORT int qskFocusChainIncrement( const QEvent* );

// some helper to work around Qt version incompatibilities
QSK_EXPORT QPointF qskMouseScenePosition( const QMouseEvent* );
QSK_EXPORT QPointF qskMousePosition( const QMouseEvent* );

#endif
