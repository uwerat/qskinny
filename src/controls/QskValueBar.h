/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_VALUE_BAR_H
#define QSK_VALUE_BAR_H

#include "QskBoundedControl.h"

class QskIntervalF;

class QSK_EXPORT QskValueBar : public QskBoundedControl
{
    Q_OBJECT

    Q_PROPERTY( Qt::Orientation orientation READ orientation
                WRITE setOrientation NOTIFY orientationChanged )

    Q_PROPERTY( qreal origin READ origin
                WRITE setOrigin RESET resetOrigin NOTIFY originChanged )

    Q_PROPERTY( qreal value READ value WRITE setValue NOTIFY valueChanged )
    Q_PROPERTY( qreal valueAsRatio READ valueAsRatio
                WRITE setValueAsRatio NOTIFY valueChanged )

    using Inherited = QskBoundedControl;

  public:
    QSK_SUBCONTROLS( Groove, ValueFill )

    QskValueBar( Qt::Orientation, QQuickItem* parent = nullptr );
    QskValueBar( Qt::Orientation, qreal min, qreal max, QQuickItem* parent = nullptr );
    QskValueBar( const QskIntervalF&, QQuickItem* parent = nullptr );
    QskValueBar( qreal min, qreal max, QQuickItem* parent = nullptr );
    QskValueBar( QQuickItem* parent = nullptr );

    ~QskValueBar() override;

    Qt::Orientation orientation() const;
    void setOrientation( Qt::Orientation orientation );

    QskAspect::Placement effectivePlacement() const override;

    void setFillGradient( const QskGradient& );
    QskGradient fillGradient() const;

    void setThickness( qreal );
    qreal thickness() const;

    void resetOrigin();
    qreal origin() const;

    qreal value() const;
    qreal valueAsRatio() const; // [0.0, 1.0]
    using QskBoundedControl::valueAsRatio;

  public Q_SLOTS:
    void setValue( qreal );
    void setValueAsRatio( qreal );
    void setOrigin( qreal );

  Q_SIGNALS:
    void orientationChanged( Qt::Orientation );
    void valueChanged( qreal );
    void originChanged( qreal );

  protected:
    QSizeF contentsSizeHint( Qt::SizeHint, const QSizeF& ) const override;
    void componentComplete() override;

  private:
    void setValueInternal( qreal value );
    void adjustBoundaries( bool increasing );
    void adjustValue();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
