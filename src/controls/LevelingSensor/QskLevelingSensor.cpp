#include "QskLevelingSensor.h"
#include <QskScaleTickmarks.h>

namespace
{
    template<typename T>
    bool compareExchange(T& dst, const T& src)
    {
        if (dst != src)
        {
            dst = src;
            return true;
        }
        return false;
    }

    template<>
    bool compareExchange<float>(float& dst, const float& src)
    {
        if (!qFuzzyCompare(dst, src))
        {
            dst = src;
            return true;
        }
        return false;
    }

    inline bool isAxis(const Qt::Axis axis) {
        return axis == Qt::XAxis || axis == Qt::YAxis || axis == Qt::ZAxis;
    }
}

QSK_SUBCONTROL(LevelingSensor, OuterDisk)
QSK_SUBCONTROL(LevelingSensor, Horizon)
QSK_SUBCONTROL(LevelingSensor, TickmarksX)
QSK_SUBCONTROL(LevelingSensor, TickmarksXLabels)
QSK_SUBCONTROL(LevelingSensor, TickmarksY)
QSK_SUBCONTROL(LevelingSensor, TickmarksYLabels)
QSK_SUBCONTROL(LevelingSensor, TickmarksZ)
QSK_SUBCONTROL(LevelingSensor, TickmarksZLabels)

#define RETURN_IF_FALSE(expr) if(!(expr)) return;

LevelingSensor::LevelingSensor(QQuickItem* const parent)
    : Inherited(parent)
{
}

void LevelingSensor::setRotation(const QVector3D& degree)
{
    if (m_rotation != degree)
    {
        setRotation(Qt::XAxis, degree.x());
        setRotation(Qt::YAxis, degree.y());
        setRotation(Qt::ZAxis, degree.z());
    }
}

void LevelingSensor::setRotation(const Qt::Axis axis, const float degree)
{
    RETURN_IF_FALSE(isAxis(axis));

    if (compareExchange(m_rotation[axis], degree))
    {
        update();
        switch(axis)
        {
        case Qt::XAxis: Q_EMIT rotationXChanged(m_rotation[axis]); break;
        case Qt::YAxis: Q_EMIT rotationYChanged(m_rotation[axis]); break;
        case Qt::ZAxis: Q_EMIT rotationZChanged(m_rotation[axis]); break;
        }
        Q_EMIT rotationChanged(m_rotation);
    }
}

void LevelingSensor::setTickmarks(const Qt::Axis axis, QskScaleTickmarks tickmarks)
{
    RETURN_IF_FALSE(isAxis(axis));

    m_tickmarks[axis] = std::move(tickmarks);
    update();
}

void LevelingSensor::setTickmarksLabels(const Qt::Axis axis, TickmarksLabels labels)
{
    RETURN_IF_FALSE(isAxis(axis));
    
    m_tickmarksLabels[axis] = std::move(labels);
    update();
}

void LevelingSensor::setAngle(const QVector3D& degree)
{
    if (compareExchange(m_angle, degree))
    {
        update();
        Q_EMIT anglesChanged(m_angle);
    }
}

void LevelingSensor::setAngle(const Qt::Axis axis, const float degree)
{
    RETURN_IF_FALSE(isAxis(axis));

    if (compareExchange(m_angle[axis], degree))
    {
        update();
        Q_EMIT anglesChanged(m_angle);
    }
}

const QskScaleTickmarks& LevelingSensor::tickmarks(Qt::Axis axis) const
{
    if (isAxis(axis))
    {
        return m_tickmarks[axis];
    }
    static const QskScaleTickmarks invalid;
    return invalid;
}

const LevelingSensor::TickmarksLabels& LevelingSensor::tickmarkLabels(Qt::Axis axis) const
{
    if (isAxis(axis))
    {
        return m_tickmarksLabels[axis];
    }
    static const LevelingSensor::TickmarksLabels invalid;
    return invalid;
}

const QVector3D& LevelingSensor::angle() const noexcept
{
    return m_angle;
}

const QVector3D& LevelingSensor::rotation() const noexcept
{
    return m_rotation;
}

#include "moc_QskLevelingSensor.cpp"