/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_PROGRESS_BAR_H
#define QSK_PROGRESS_BAR_H

#include "QskProgressIndicator.h"

class QSK_EXPORT QskProgressBar : public QskProgressIndicator
{
    Q_OBJECT

    Q_PROPERTY( Qt::Orientation orientation READ orientation
        WRITE setOrientation NOTIFY orientationChanged )

    using Inherited = QskProgressIndicator;

  public:
    QSK_SUBCONTROLS( Groove, Fill, GrooveStopIndicator )

    QskProgressBar( Qt::Orientation, QQuickItem* parent = nullptr );
    QskProgressBar( Qt::Orientation, qreal min, qreal max, QQuickItem* parent = nullptr );
    QskProgressBar( const QskIntervalF&, QQuickItem* parent = nullptr );
    QskProgressBar( qreal min, qreal max, QQuickItem* parent = nullptr );
    QskProgressBar( QQuickItem* parent = nullptr );

    ~QskProgressBar() override;

    QskAspect::Variation effectiveVariation() const override;

    Qt::Orientation orientation() const;
    void setOrientation( Qt::Orientation orientation );

  Q_SIGNALS:
    void orientationChanged( Qt::Orientation );

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
