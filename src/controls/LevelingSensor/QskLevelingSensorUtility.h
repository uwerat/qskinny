#pragma once

#include <qmath.h>
#include <qmatrix4x4.h>

#include <QskScaleTickmarks.h>

// create a homogenous transformation matrix
inline Q_REQUIRED_RESULT QMatrix4x4 matrix_deg(
    float rX = 0.0f,
    float rY = 0.0f,
    float rZ = 0.0f,
    float tX = 0.0f,
    float tY = 0.0f,
    float tZ = 0.0f
)
{
    // Convert rotation angles to radians
    float rotationX = qDegreesToRadians(rX);
    float rotationY = qDegreesToRadians(rY);
    float rotationZ = qDegreesToRadians(rZ);

    // Calculate sin and cos of the rotation angles
    float cosX = qCos(rotationX);
    float sinX = qSin(rotationX);
    float cosY = qCos(rotationY);
    float sinY = qSin(rotationY);
    float cosZ = qCos(rotationZ);
    float sinZ = qSin(rotationZ);

    // Create the transform matrix
    return QMatrix4x4(
        cosY * cosZ, sinX * sinY * cosZ - cosX * sinZ, cosX * sinY * cosZ + sinX * sinZ, tX,
        cosY * sinZ, sinX * sinY * sinZ + cosX * cosZ, cosX * sinY * sinZ - sinX * cosZ, tY,
        -sinY, sinX * cosY, cosX * cosY, tZ,
        0, 0, 0, 1
    );
}

template<typename T>
inline bool compare_exchange(T& dst, const T& src)
{
    if (dst != src)
    {
        dst = src;
        return true;
    }
    return false;
}

template<>
inline bool compare_exchange<float>(float& dst, const float& src)
{
    if (!qFuzzyCompare(dst, src))
    {
        dst = src;
        return true;
    }
    return false;
}

template<>
inline bool compare_exchange<qreal>(qreal& dst, const qreal& src)
{
    if (!qFuzzyCompare(dst, src))
    {
        dst = src;
        return true;
    }
    return false;
}

inline QskScaleTickmarks filtered(const QskScaleTickmarks& tickmarks, const std::function<bool(QskScaleTickmarks::TickType, qreal)>& predicate)
{
    QskScaleTickmarks result;
    QVector<qreal> ticks[3];

    using T = QskScaleTickmarks::TickType;
    for (auto type : { T::MinorTick, T::MediumTick, T::MajorTick })
    {
        for (const auto tick : tickmarks.ticks(type))
        {
            if (predicate(type, tick))
            {
                ticks[type] << tick;
            }
        }
    }

    result.setMinorTicks(ticks[QskScaleTickmarks::MinorTick]);
    result.setMediumTicks(ticks[QskScaleTickmarks::MediumTick]);
    result.setMajorTicks(ticks[QskScaleTickmarks::MajorTick]);
    return result;
}