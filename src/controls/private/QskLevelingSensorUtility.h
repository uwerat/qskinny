/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_LEVELING_SENSOR_UTILITY_H
#define QSK_LEVELING_SENSOR_UTILITY_H

#include <qmath.h>
#include <qmatrix4x4.h>
#include <qtransform.h>

#include <QskFunctions.h>
#include <QskScaleTickmarks.h>

Q_REQUIRED_RESULT inline QMatrix4x4 matrix_deg( float rZ = 0.0f, float tX = 0.0f, float tY = 0.0f )
{
    QTransform transform;
    transform.translate( tX, tY );
    transform.rotate( rZ, Qt::ZAxis );
    return transform;
}

template< typename T >
Q_REQUIRED_RESULT inline bool compareExchange( T& dst, const T& src )
{
    if ( dst != src )
    {
        dst = src;
        return true;
    }
    return false;
}

template<>
Q_REQUIRED_RESULT inline bool compareExchange< float >( float& dst, const float& src )
{
    if ( !qskFuzzyCompare( dst, src ) )
    {
        dst = src;
        return true;
    }
    return false;
}

template<>
Q_REQUIRED_RESULT inline bool compareExchange< qreal >( qreal& dst, const qreal& src )
{
    if ( !qskFuzzyCompare( dst, src ) )
    {
        dst = src;
        return true;
    }
    return false;
}

Q_REQUIRED_RESULT inline QskScaleTickmarks filtered( const QskScaleTickmarks& tickmarks,
    const std::function< bool( QskScaleTickmarks::TickType, qreal ) >& predicate )
{
    QskScaleTickmarks result;
    QVector< qreal > ticks[ 3 ];

    using T = QskScaleTickmarks::TickType;
    for ( auto type : { T::MinorTick, T::MediumTick, T::MajorTick } )
    {
        for ( const auto tick : tickmarks.ticks( type ) )
        {
            if ( predicate( type, tick ) )
            {
                ticks[ type ] << tick;
            }
        }
    }

    result.setMinorTicks( ticks[ QskScaleTickmarks::MinorTick ] );
    result.setMediumTicks( ticks[ QskScaleTickmarks::MediumTick ] );
    result.setMajorTicks( ticks[ QskScaleTickmarks::MajorTick ] );
    return result;
}

#endif