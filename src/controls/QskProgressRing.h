/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_PROGRESS_RING_H
#define QSK_PROGRESS_RING_H

#include "QskProgressIndicator.h"

class QSK_EXPORT QskProgressRing : public QskProgressIndicator
{
    Q_OBJECT

    Q_PROPERTY( Size size READ size
        WRITE setSize NOTIFY sizeChanged )

    using Inherited = QskProgressIndicator;

  public:
    QSK_SUBCONTROLS( Groove, Fill )

    enum Size : qint8
    {
        SmallSize  = -1,
        NormalSize = 0,
        LargeSize  = 1
    };
    Q_ENUM( Size )

    QskProgressRing( QQuickItem* parent = nullptr );
    QskProgressRing( qreal min, qreal max, QQuickItem* parent = nullptr );
    QskProgressRing( const QskIntervalF&, QQuickItem* parent = nullptr );

    ~QskProgressRing() override;

    QskAspect::Variation effectiveVariation() const override;

    void setSize( Size );
    Size size() const;

  Q_SIGNALS:
    void sizeChanged( Size );

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif
