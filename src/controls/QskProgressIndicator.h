/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_PROGRESS_INDICATOR_H
#define QSK_PROGRESS_INDICATOR_H

#include "QskBoundedControl.h"

class QskIntervalF;

class QSK_EXPORT QskProgressIndicator : public QskBoundedControl
{
    Q_OBJECT

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
    QSK_SUBCONTROLS( Groove, Fill )

    QskProgressIndicator( QQuickItem* parent = nullptr );
    QskProgressIndicator( qreal min, qreal max, QQuickItem* parent = nullptr );
    QskProgressIndicator( const QskIntervalF&, QQuickItem* parent = nullptr );

    ~QskProgressIndicator() override;

    bool isIndeterminate() const;
    void setIndeterminate( bool on = true );

    void setFillGradient( const QskGradient& );
    void resetFillGradient();
    QskGradient fillGradient() const;

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
