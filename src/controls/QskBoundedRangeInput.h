/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOUNDED_RANGE_INPUT_H
#define QSK_BOUNDED_RANGE_INPUT_H

#include "QskBoundedInput.h"
#include "QskIntervalF.h"

class QSK_EXPORT QskBoundedRangeInput : public QskBoundedInput
{
    Q_OBJECT

    Q_PROPERTY( qreal lowerValue READ lowerValue
        WRITE setLowerValue NOTIFY lowerValueChanged )

    Q_PROPERTY( qreal upperValue READ upperValue
        WRITE setUpperValue NOTIFY upperValueChanged )

    Q_PROPERTY( QskIntervalF range READ range WRITE setRange
        RESET resetRange NOTIFY rangeChanged )

    using Inherited = QskBoundedInput;

  public:
    QskBoundedRangeInput( QQuickItem* parent = nullptr );
    QskBoundedRangeInput( const QskIntervalF&, QQuickItem* parent = nullptr );
    ~QskBoundedRangeInput() override;

    void setRange( qreal lowerValue, qreal upperValue );
    void resetRange();

    QskIntervalF range() const;

    qreal lowerValue() const;
    qreal upperValue() const;

    // [0.0, 1.0]
    qreal lowerValueAsRatio() const;
    qreal upperValueAsRatio() const;

  public Q_SLOTS:
    void setRange( const QskIntervalF& );

    void setLowerValue( qreal );
    void setUpperValue( qreal );

    void setLowerValueAsRatio( qreal );
    void setUpperValueAsRatio( qreal );

  Q_SIGNALS:
    void rangeChanged( const QskIntervalF& );
    void lowerValueChanged( qreal );
    void upperValueChanged( qreal );

  protected:
    virtual QskIntervalF fixupRange( const QskIntervalF& ) const;

    void alignInput() override;

  private:
    void setRangeInternal( const QskIntervalF& );
    void adjustValue();

    QskIntervalF m_range;
};

#endif
