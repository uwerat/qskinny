/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_VARIANT_ANIMATOR_H
#define QSK_VARIANT_ANIMATOR_H

#include "QskGlobal.h"
#include "QskAnimator.h"
#include <QVariant>

class QSK_EXPORT QskVariantAnimator : public QskAnimator
{
public:
    QskVariantAnimator();
    virtual ~QskVariantAnimator();

    void setCurrentValue( const QVariant& );
    QVariant currentValue() const;

    void setStartValue( const QVariant& );
    QVariant startValue() const;

    void setEndValue( const QVariant& );
    QVariant endValue() const;

protected:
    virtual void setup() override;
    virtual void advance( qreal value ) override;
    virtual void done() override;

private:
    QVariant m_startValue;
    QVariant m_endValue;
    QVariant m_currentValue;

    void( *m_interpolator )();
};

inline QVariant QskVariantAnimator::startValue() const
{
    return m_startValue;
}

inline QVariant QskVariantAnimator::endValue() const
{
    return m_endValue;
}

inline QVariant QskVariantAnimator::currentValue() const
{
    return m_currentValue;
}

#endif
