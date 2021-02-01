/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_HINT_ANIMATOR_H
#define QSK_HINT_ANIMATOR_H

#include "QskAspect.h"
#include "QskVariantAnimator.h"
#include "QskAnimationHint.h"

#include <qpointer.h>

class QskControl;

class QSK_EXPORT QskHintAnimator : public QskVariantAnimator
{
    using Inherited = QskVariantAnimator;

  public:
    QskHintAnimator();
    ~QskHintAnimator() override;

    void setAspect( QskAspect );
    QskAspect aspect() const;

    void setControl( QskControl* );
    QskControl* control() const;

    void setUpdateFlags( QskAnimationHint::UpdateFlags );
    QskAnimationHint::UpdateFlags updateFlags() const;

    void advance( qreal value ) override;

  private:
    QskAspect m_aspect;
    QskAnimationHint::UpdateFlags m_updateFlags;
    QPointer< QskControl > m_control;
};

class QSK_EXPORT QskHintAnimatorTable
{
  public:
    QskHintAnimatorTable();
    ~QskHintAnimatorTable();

    void start( QskControl*, QskAspect,
        QskAnimationHint, const QVariant& from, const QVariant& to );

    const QskHintAnimator* animator( QskAspect ) const;
    QVariant currentValue( QskAspect ) const;

    bool cleanup();

  private:
    void reset();

    class PrivateData;
    PrivateData* m_data;
};

inline QskAspect QskHintAnimator::aspect() const
{
    return m_aspect;
}

inline QskAnimationHint::UpdateFlags QskHintAnimator::updateFlags() const
{
    return m_updateFlags;
}

inline QskControl* QskHintAnimator::control() const
{
    return m_control;
}

#endif
