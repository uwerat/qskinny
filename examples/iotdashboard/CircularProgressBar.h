/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskBoundedControl.h>

class CircularProgressBar : public QskBoundedControl
{
    Q_OBJECT

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

    CircularProgressBar( qreal min, qreal max, QQuickItem* parent = nullptr );
    CircularProgressBar( QQuickItem* parent = nullptr );
    ~CircularProgressBar() override;

    bool isIndeterminate() const;
    void setIndeterminate( bool on = true );

    void resetOrigin();
    qreal origin() const;

    qreal value() const;
    qreal valueAsRatio() const; // [0.0, 1.0]

  public Q_SLOTS:
    void setValue( qreal );
    void setValueAsRatio( qreal );
    void setOrigin( qreal );

  Q_SIGNALS:
    void indeterminateChanged( bool );
    void valueChanged( qreal );
    void originChanged( qreal );

  protected:
    void componentComplete() override;

  private:
    void setValueInternal( qreal value );
    void adjustValue();

    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};
