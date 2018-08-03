/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_HINT_ANIMATOR_H
#define QSK_HINT_ANIMATOR_H

#include "QskAspect.h"
#include "QskVariantAnimator.h"

#include <qpointer.h>

class QskControl;
class QskAnimationHint;

class QSK_EXPORT QskHintAnimator : public QskVariantAnimator
{
    using Inherited = QskVariantAnimator;

  public:
    QskHintAnimator();
    ~QskHintAnimator() override;

    void setAspect( QskAspect::Aspect );
    QskAspect::Aspect aspect() const;

    void setControl( QskControl* );
    QskControl* control() const;

    void advance( qreal value ) override;

  private:
    QskAspect::Aspect m_aspect;
    QPointer< QskControl > m_control;
};

class QSK_EXPORT QskHintAnimatorTable
{
  public:
    QskHintAnimatorTable();
    ~QskHintAnimatorTable();

    void start( QskControl*, QskAspect::Aspect,
        QskAnimationHint, const QVariant& from, const QVariant& to );

    const QskHintAnimator* animator( QskAspect::Aspect aspect ) const;
    QVariant currentValue( QskAspect::Aspect ) const;

    bool cleanup();

  private:
    void reset();

    class PrivateData;
    PrivateData* m_data;
};

inline QskAspect::Aspect QskHintAnimator::aspect() const
{
    return m_aspect;
}

inline QskControl* QskHintAnimator::control() const
{
    return m_control;
}

#endif
