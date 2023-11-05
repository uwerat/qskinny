/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_TICKMARKS_METRICS_H
#define QSK_TICKMARKS_METRICS_H

#include "QskFunctions.h"

#include <qmetatype.h>

class QSK_EXPORT QskTickmarksMetrics
{
    Q_GADGET
    Q_PROPERTY( qreal majorRatio READ majorRatio WRITE setMajorRatio )
    Q_PROPERTY( qreal mediumRatio READ mediumRatio WRITE setMediumRatio )
    Q_PROPERTY( qreal minorRatio READ minorRatio WRITE setMinorRatio )

  public:
    constexpr QskTickmarksMetrics() noexcept = default;
    constexpr QskTickmarksMetrics( qreal minorRatio, qreal mediumRatio, qreal majorRatio ) noexcept;
    constexpr QskTickmarksMetrics( const QskTickmarksMetrics& ) noexcept = default;
    constexpr QskTickmarksMetrics( QskTickmarksMetrics&& ) noexcept = default;

    constexpr QskTickmarksMetrics& operator=( const QskTickmarksMetrics& ) noexcept = default;
    constexpr QskTickmarksMetrics& operator=( QskTickmarksMetrics&& ) noexcept = default;

    constexpr Q_REQUIRED_RESULT bool operator==( const QskTickmarksMetrics& rhs ) const noexcept;
    constexpr Q_REQUIRED_RESULT bool operator!=( const QskTickmarksMetrics& rhs ) const noexcept;

    constexpr Q_REQUIRED_RESULT qreal majorRatio() const noexcept;
    constexpr Q_REQUIRED_RESULT qreal mediumRatio() const noexcept;
    constexpr Q_REQUIRED_RESULT qreal minorRatio() const noexcept;

    constexpr void setMajorRatio( qreal ratio ) noexcept;
    constexpr void setMediumRatio( qreal ratio ) noexcept;
    constexpr void setMinorRatio( qreal ratio ) noexcept;

    constexpr Q_REQUIRED_RESULT qreal operator[]( int index ) const noexcept;
    constexpr Q_REQUIRED_RESULT qreal& operator[]( int index ) noexcept;

  private:
    qreal m_minorRatio = 0.7;
    qreal m_mediumRatio = 0.85;
    qreal m_majorRatio = 1.0;
};

inline constexpr QskTickmarksMetrics::QskTickmarksMetrics(
    qreal minorRatio, qreal mediumRatio, qreal majorRatio ) noexcept
    : m_minorRatio( minorRatio )
    , m_mediumRatio( mediumRatio )
    , m_majorRatio( majorRatio )
{
}

inline constexpr qreal QskTickmarksMetrics::majorRatio() const noexcept
{
    return m_majorRatio;
}

inline constexpr qreal QskTickmarksMetrics::mediumRatio() const noexcept
{
    return m_mediumRatio;
}

inline constexpr qreal QskTickmarksMetrics::minorRatio() const noexcept
{
    return m_minorRatio;
}

inline constexpr void QskTickmarksMetrics::setMajorRatio( qreal ratio ) noexcept
{
    m_majorRatio = qBound( 0.0, ratio, 1.0 );
}

inline constexpr void QskTickmarksMetrics::setMediumRatio( qreal ratio ) noexcept
{
    m_mediumRatio = qBound( 0.0, ratio, 1.0 );
}

inline constexpr void QskTickmarksMetrics::setMinorRatio( qreal ratio ) noexcept
{
    m_minorRatio = qBound( 0.0, ratio, 1.0 );
}

inline constexpr bool QskTickmarksMetrics::operator==(
    const QskTickmarksMetrics& rhs ) const noexcept
{
    return qskFuzzyCompare( m_minorRatio, rhs.m_minorRatio ) &&
           qskFuzzyCompare( m_mediumRatio, rhs.m_mediumRatio ) &&
           qskFuzzyCompare( m_majorRatio, rhs.m_majorRatio );
}

inline constexpr bool QskTickmarksMetrics::operator!=(
    const QskTickmarksMetrics& rhs ) const noexcept
{
    return !( *this == rhs );
}

inline constexpr qreal QskTickmarksMetrics::operator[]( int index ) const noexcept
{
    index = index % 3;

    switch ( index )
    {
        case 0:
            return m_minorRatio;
        case 1:
            return m_mediumRatio;
        default:
            return m_majorRatio;
    }
}

inline constexpr qreal& QskTickmarksMetrics::operator[]( int index ) noexcept
{
    index = index % 3;

    switch ( index )
    {
        case 0:
            return m_minorRatio;
        case 1:
            return m_mediumRatio;
        default:
            return m_majorRatio;
    }
}

#endif