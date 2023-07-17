#pragma once

#include <QskControl.h>
#include <QskScaleTickmarks.h>
#include <QVector3D>
#include <QskAspect.h>

class QSK_EXPORT QskLevelingSensor : public QskControl
{
    Q_OBJECT
        using Inherited = QskControl;
public:
    QSK_SUBCONTROLS(
        OuterDisk,
        Horizon,
        TickmarksX,
        TickmarksXLabels,
        TickmarksY,
        TickmarksYLabels,
        TickmarksZ,
        TickmarksZLabels)
        using Tickmarks = QskScaleTickmarks;
        using TickmarksLabels = QVector<QPair<qreal, QString>>;
    explicit QskLevelingSensor(QQuickItem* parent = nullptr);
public Q_SLOTS:
    void setRotation(const QVector3D& degree);
    void setRotation(Qt::Axis axis, float degree);
    void setTickmarks(Qt::Axis axis, Tickmarks tickmarks);
    void setTickmarksLabels(Qt::Axis axis, TickmarksLabels labels);
    void setAngle(const QVector3D& degree);
    void setAngle(Qt::Axis axis, float degree);
Q_SIGNALS:
    void rotationXChanged(qreal degree);
    void rotationYChanged(qreal degree);
    void rotationZChanged(qreal degree);
    void rotationChanged(const QVector3D& degree);
    void anglesChanged(const QVector3D& degree);
public:
    Q_REQUIRED_RESULT const QVector3D& rotation() const noexcept;
    Q_REQUIRED_RESULT const Tickmarks& tickmarks(Qt::Axis axis) const;
    Q_REQUIRED_RESULT const TickmarksLabels& tickmarkLabels(Qt::Axis axis) const;
    Q_REQUIRED_RESULT const QVector3D& angle() const noexcept;
private:
    QVector3D m_rotation;
    QVector3D m_angle = { 45,45,45 };
    Tickmarks m_tickmarks[3];
    TickmarksLabels m_tickmarksLabels[3];
};