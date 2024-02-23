/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_BOUNDED_VALUE_INPUT_H
#define QSK_BOUNDED_VALUE_INPUT_H

#include "QskBoundedInput.h"

class QSK_EXPORT QskBoundedValueInput : public QskBoundedInput
{
    Q_OBJECT

    Q_PROPERTY( qreal value READ value
        WRITE setValue NOTIFY valueChanged USER true )

    Q_PROPERTY( qreal valueAsRatio READ valueAsRatio
        WRITE setValueAsRatio NOTIFY valueChanged )

    using Inherited = QskBoundedInput;

  public:
    QskBoundedValueInput( QQuickItem* parent = nullptr );
    ~QskBoundedValueInput() override;

    qreal value() const;

    // [0.0, 1.0]
    qreal valueAsRatio() const;
    using QskBoundedInput::valueAsRatio;

  public Q_SLOTS:
    void setValue( qreal );
    void setValueAsRatio( qreal );
    void increment( qreal offset ) override;

  Q_SIGNALS:
    void valueChanged( qreal );

  protected:
    virtual qreal fixupValue( qreal value ) const;
    void alignInput() override;

  private:
    void setValueInternal( qreal value );
    void adjustValue();

    qreal m_value = 0.0;
};

#endif
