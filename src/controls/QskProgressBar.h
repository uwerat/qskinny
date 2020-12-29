/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_PROGRESS_BAR_H
#define QSK_PROGRESS_BAR_H

#include "QskBoundedControl.h"

class QskIntervalF;

class QSK_EXPORT QskProgressBar : public QskBoundedControl
{
    Q_OBJECT

    Q_PROPERTY( Qt::Orientation orientation READ orientation
        WRITE setOrientation NOTIFY orientationChanged )

    Q_PROPERTY( qreal extent READ extent
        WRITE setExtent RESET resetExtent NOTIFY extentChanged )

    Q_PROPERTY( bool indeterminate READ isIndeterminate
        WRITE setIndeterminate NOTIFY indeterminateChanged )

    Q_PROPERTY( qreal origin READ origin
        WRITE setOrigin RESET resetOrigin NOTIFY originChanged )

    Q_PROPERTY( qreal value READ value WRITE setValue NOTIFY valueChanged )
    Q_PROPERTY( qreal valueAsRatio READ valueAsRatio
        WRITE setValueAsRatio NOTIFY valueChanged )

    using Inherited = QskBoundedControl;

  public:
    QSK_SUBCONTROLS( Groove, Bar )

    QskProgressBar( Qt::Orientation, QQuickItem* parent = nullptr );
    QskProgressBar( Qt::Orientation, qreal min, qreal max, QQuickItem* parent = nullptr );
    QskProgressBar( const QskIntervalF&, QQuickItem* parent = nullptr );
    QskProgressBar( qreal min, qreal max, QQuickItem* parent = nullptr );
    QskProgressBar( QQuickItem* parent = nullptr );

    ~QskProgressBar() override;

    Qt::Orientation orientation() const;
    void setOrientation( Qt::Orientation orientation );

    bool isIndeterminate() const;
    void setIndeterminate( bool on = true );

    QskAspect::Placement effectivePlacement() const override;

    void setBarGradient( const QskGradient& );
    void resetBarGradient();
    QskGradient barGradient() const;

    void setExtent( qreal );
    void resetExtent();
    qreal extent() const;

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
    void extentChanged( qreal );
    void indeterminateChanged( bool );
    void valueChanged( qreal );
    void originChanged( qreal );

  protected:
    void componentComplete() override;
    void itemChange( ItemChange, const ItemChangeData& ) override;

  private:
    void setValueInternal( qreal value );
    void adjustBoundaries( bool increasing );
    void adjustValue();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
