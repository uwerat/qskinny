/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SEPARATOR_H
#define QSK_SEPARATOR_H

#include "QskControl.h"

class QSK_EXPORT QskSeparator : public QskControl
{
    Q_OBJECT

    Q_PROPERTY( Qt::Orientation orientation READ orientation
        WRITE setOrientation NOTIFY orientationChanged )

    Q_PROPERTY( qreal extent READ extent
        WRITE setExtent RESET resetExtent NOTIFY extentChanged )

    using Inherited = QskControl;

  public:
    QSK_SUBCONTROLS( Panel )

    QskSeparator( QQuickItem* parent = nullptr );
    QskSeparator( Qt::Orientation, QQuickItem* parent = nullptr );

    ~QskSeparator() override;

    void setOrientation( Qt::Orientation );
    Qt::Orientation orientation() const;

    void setExtent( qreal );
    void resetExtent();
    qreal extent() const;

    QskAspect::Variation effectiveVariation() const override;

  Q_SIGNALS:
    void orientationChanged( Qt::Orientation );
    void extentChanged( qreal );

  private:
    Qt::Orientation m_orientation;
};

#endif
