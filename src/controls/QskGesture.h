/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_GESTURE_H
#define QSK_GESTURE_H

#include "QskGlobal.h"

#include <qmetatype.h>
#include <qpoint.h>

class QSK_EXPORT QskGesture
{
    Q_GADGET

  public:
    enum Type
    {
        NoType = -1,

        Tap,
        TapAndHold,
        Pan,
#if 0
        // TODO
        Pinch,
#endif
        Swipe,

        CustomType = 16
    };
    Q_ENUM( Type )

    enum State
    {
        NoGesture,

        Started,
        Updated,
        Finished,
        Canceled
    };
    Q_ENUM( State )

    virtual ~QskGesture();

    Type type() const { return m_type; }

    void setState( State );
    inline State state() const { return m_state; }

  protected:
    QskGesture( Type type );

    const Type m_type;
    State m_state;
};

class QSK_EXPORT QskTapGesture : public QskGesture
{
    using Inherited = QskGesture;

  public:
    QskTapGesture();
    ~QskTapGesture() override;

    void setPosition( const QPointF& pos );
    inline QPointF position() const { return m_position; }

  private:
    QPointF m_position;
};

class QSK_EXPORT QskTapAndHoldGesture : public QskGesture
{
    using Inherited = QskGesture;

  public:
    QskTapAndHoldGesture();
    ~QskTapAndHoldGesture() override;

    void setPosition( const QPointF& pos );
    inline QPointF position() const { return m_position; }

    void setTimeout( int msecs );
    inline int timeout() const { return m_timeout; }

  private:
    QPointF m_position;
    int m_timeout;
};

class QSK_EXPORT QskPanGesture : public QskGesture
{
    using Inherited = QskGesture;

  public:
    QskPanGesture();
    ~QskPanGesture() override;

    void setVelocity( qreal );
    inline qreal velocity() const { return m_velocity; }

    void setOrigin( const QPointF& );
    inline const QPointF& origin() const { return m_origin; }

    void setLastPosition( const QPointF& );
    inline QPointF lastPosition() const { return m_lastPosition; }

    void setPosition( const QPointF& );
    inline const QPointF& position() const { return m_position; }

    void setAngle( qreal degrees );
    inline qreal angle() const { return m_angle; }

    inline QPointF delta() const { return m_position - m_lastPosition; }

  private:
    qreal m_angle;
    qreal m_velocity;

    QPointF m_origin;
    QPointF m_lastPosition;
    QPointF m_position;
};

class QSK_EXPORT QskSwipeGesture : public QskGesture
{
    using Inherited = QskGesture;

  public:
    QskSwipeGesture();
    ~QskSwipeGesture() override;

    void setVelocity( qreal velocity );
    inline qreal velocity() const { return m_velocity; }

    void setAngle( qreal angle );
    inline qreal angle() const;

  private:
    qreal m_velocity;
    qreal m_angle;
};

#endif
