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
    QskHintAnimator() noexcept;
    QskHintAnimator( QskAspect, int index ) noexcept;

    ~QskHintAnimator() override;

    void setAspect( QskAspect ) noexcept;
    QskAspect aspect() const noexcept;

    void setIndex( int ) noexcept;
    int index() const noexcept;

    void setControl( QskControl* ) noexcept;
    QskControl* control() const noexcept;

    void setUpdateFlags( QskAnimationHint::UpdateFlags ) noexcept;
    QskAnimationHint::UpdateFlags updateFlags() const noexcept;

    void advance( qreal value ) override;

    bool operator<( const QskHintAnimator& ) const noexcept;

  private:
    QskAspect m_aspect;
    int m_index;
    QskAnimationHint::UpdateFlags m_updateFlags;
    QPointer< QskControl > m_control;
};

#ifndef QT_NO_DEBUG_STREAM

class QDebug;
QSK_EXPORT QDebug operator<<( QDebug, const QskHintAnimator& );

#endif

class QSK_EXPORT QskHintAnimatorTable
{
  public:
    QskHintAnimatorTable();
    ~QskHintAnimatorTable();

    void start( QskControl*, QskAspect, int index,
        QskAnimationHint, const QVariant& from, const QVariant& to );

    const QskHintAnimator* animator( QskAspect, int index = -1 ) const;
    QVariant currentValue( QskAspect, int index = -1 ) const;

    bool cleanup();
    bool isEmpty() const;

  private:
    void reset();

    class PrivateData;
    PrivateData* m_data = nullptr;
};

inline QskAspect QskHintAnimator::aspect() const noexcept
{
    return m_aspect;
}

inline int QskHintAnimator::index() const noexcept
{
    return m_index;
}

inline QskAnimationHint::UpdateFlags QskHintAnimator::updateFlags() const noexcept
{
    return m_updateFlags;
}

inline QskControl* QskHintAnimator::control() const noexcept
{
    return m_control;
}

inline bool QskHintAnimator::operator<( const QskHintAnimator& other ) const noexcept
{
    return m_aspect < other.m_aspect;
}

inline bool QskHintAnimatorTable::isEmpty() const
{
    return m_data == nullptr;
}

#endif
