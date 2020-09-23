/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_BOUNDED_CONTROL_H
#define QSK_BOUNDED_CONTROL_H

#include "QskControl.h"

class QskIntervalF;

class QSK_EXPORT QskBoundedControl : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( qreal minimum READ minimum WRITE setMinimum NOTIFY minimumChanged )
    Q_PROPERTY( qreal maximum READ maximum WRITE setMaximum NOTIFY maximumChanged )
    Q_PROPERTY( QskIntervalF boundaries READ boundaries
        WRITE setBoundaries NOTIFY boundariesChanged )

    using Inherited = QskControl;

  public:
    ~QskBoundedControl() override;

    qreal minimum() const;
    qreal maximum() const;

    qreal boundaryLength() const;

    void setBoundaries( qreal min, qreal max );
    QskIntervalF boundaries() const;

    qreal boundedValue( qreal ) const;

    qreal valueAsRatio( qreal ) const;
    qreal valueFromRatio( qreal ) const;

  public Q_SLOTS:
    void setMinimum( qreal );
    void setMaximum( qreal );
    void setBoundaries( const QskIntervalF& );

  Q_SIGNALS:
    void minimumChanged( qreal );
    void maximumChanged( qreal );
    void boundariesChanged( const QskIntervalF&  );

  protected:
    QskBoundedControl( QQuickItem* parent = nullptr );
    QskBoundedControl( qreal min, qreal max, QQuickItem* parent = nullptr );

    void componentComplete() override;

    void adjustBoundaries( bool increasing );

  private:
    qreal m_minimum;
    qreal m_maximum;
};

#endif
