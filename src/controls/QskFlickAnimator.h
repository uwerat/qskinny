/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_FLICK_ANIMATOR_H
#define QSK_FLICK_ANIMATOR_H 1

#include "QskAnimator.h"

class QSK_EXPORT QskFlickAnimator : public QskAnimator
{
  public:
    QskFlickAnimator();
    ~QskFlickAnimator() override;

    void setAngle( qreal degrees );
    qreal angle() const;

    void setVelocity( qreal );
    qreal velocity() const;

    qreal animatedVelocity() const;

    void flick( qreal degrees, qreal velocity );
    void accelerate( qreal degrees, qreal velocity );

  protected:
    void setup() override;
    void advance( qreal value ) override final;
    void done() override;

    virtual void translate( qreal dx, qreal dy ) = 0;

  private:
    qreal m_velocity[ 2 ];

    qreal m_degrees;
    qreal m_cos;
    qreal m_sin;

    int m_elapsed;
};

inline qreal QskFlickAnimator::angle() const
{
    return m_degrees;
}

inline qreal QskFlickAnimator::velocity() const
{
    return m_velocity[ 0 ];
}

inline qreal QskFlickAnimator::animatedVelocity() const
{
    return m_velocity[ 1 ];
}

#endif
