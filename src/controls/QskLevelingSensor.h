/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_LEVELING_SENSOR_H
#define QSK_LEVELING_SENSOR_H

#include <QskControl.h>
#include <QskScaleTickmarks.h>
#include <memory>

class QSK_EXPORT QskLevelingSensor : public QskControl
{
    Q_OBJECT
    using Inherited = QskControl;

  public:
    QSK_SUBCONTROLS( OuterDisk, Horizon, TickmarksX, TickmarksXLabels, TickmarksY, TickmarksYLabels,
        TickmarksZ, TickmarksZLabels )

    using Tickmarks = QskScaleTickmarks;
    using TickmarksLabels = QVector< QPair< qreal, QString > >;

    explicit QskLevelingSensor( QQuickItem* parent = nullptr );
    ~QskLevelingSensor();

  public Q_SLOTS:
    void setTickmarks( Qt::Axis axis, Tickmarks tickmarks );
    void setTickmarksLabels( Qt::Axis axis, TickmarksLabels labels );
    void setAngle( const QVector3D& degrees );
    void setSubControlRotation( QskAspect::Subcontrol subControl, const QVector3D& degrees );

  Q_SIGNALS:
    void anglesChanged( const QVector3D& degree );
    void subControlRotationChanged( QskAspect::Subcontrol subControl, const QVector3D& degrees );

  public:
    Q_REQUIRED_RESULT Tickmarks tickmarks( Qt::Axis axis ) const;
    Q_REQUIRED_RESULT TickmarksLabels tickmarkLabels( Qt::Axis axis ) const;
    Q_REQUIRED_RESULT QVector3D angles() const;
    Q_REQUIRED_RESULT QVector3D subControlRotation( QskAspect::Subcontrol subControl ) const;

  private:
    class PrivateData;
    std::unique_ptr< PrivateData > m_data;
};

#endif