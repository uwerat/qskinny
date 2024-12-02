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

    Q_PROPERTY( QString valueText READ valueText NOTIFY valueChanged )

    Q_PROPERTY( int decimals READ decimals
        WRITE setDecimals NOTIFY decimalsChanged )

    using Inherited = QskBoundedInput;

  public:
    QskBoundedValueInput( QQuickItem* parent = nullptr );
    ~QskBoundedValueInput() override;

    qreal value() const;

    // [0.0, 1.0]
    qreal valueAsRatio() const;
    using QskBoundedInput::valueAsRatio;

    QString valueText() const;
    virtual QString textFromValue( qreal ) const;

    void setDecimals( int );
    int decimals() const;

  public Q_SLOTS:
    void setValue( qreal );
    void setValueAsRatio( qreal );
    void increment( qreal ) override;

  Q_SIGNALS:
    void valueChanged( qreal );
    void decimalsChanged( int );

  protected:
    void keyPressEvent( QKeyEvent* ) override;

    virtual qreal fixupValue( qreal ) const;
    void alignInput() override;

  private:
    void setValueInternal( qreal );

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
