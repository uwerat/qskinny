/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_TICKMARKS_METRICS_H
#define QSK_TICKMARKS_METRICS_H

#include "QskFunctions.h"

#include <qmetatype.h>

class QSK_EXPORT QskGraduationMetrics
{
    Q_GADGET
    Q_PROPERTY( qreal majorRatio READ majorRatio WRITE setMajorRatio )
    Q_PROPERTY( qreal mediumRatio READ mediumRatio WRITE setMediumRatio )
    Q_PROPERTY( qreal minorRatio READ minorRatio WRITE setMinorRatio )

  public:
    constexpr QskGraduationMetrics() noexcept = default;
    constexpr QskGraduationMetrics( qreal minorRatio, qreal mediumRatio, qreal majorRatio ) noexcept;
    constexpr QskGraduationMetrics( const QskGraduationMetrics& ) noexcept = default;
    constexpr QskGraduationMetrics( QskGraduationMetrics&& ) noexcept = default;

    constexpr QskGraduationMetrics& operator=( const QskGraduationMetrics& ) noexcept = default;
    constexpr QskGraduationMetrics& operator=( QskGraduationMetrics&& ) noexcept = default;

    Q_REQUIRED_RESULT constexpr bool operator==( const QskGraduationMetrics& rhs ) const noexcept;
    Q_REQUIRED_RESULT constexpr bool operator!=( const QskGraduationMetrics& rhs ) const noexcept;

    Q_REQUIRED_RESULT constexpr qreal majorRatio() const noexcept;
    Q_REQUIRED_RESULT constexpr qreal mediumRatio() const noexcept;
    Q_REQUIRED_RESULT constexpr qreal minorRatio() const noexcept;

    constexpr void setMajorRatio( qreal ratio ) noexcept;
    constexpr void setMediumRatio( qreal ratio ) noexcept;
    constexpr void setMinorRatio( qreal ratio ) noexcept;

    Q_REQUIRED_RESULT constexpr qreal ratio( int index ) const noexcept;
    constexpr void setRatio( int index, qreal ratio ) noexcept;

    Q_REQUIRED_RESULT QskGraduationMetrics interpolated(
        const QskGraduationMetrics&, qreal progress ) const noexcept;
    Q_REQUIRED_RESULT static QVariant interpolate(
        const QskGraduationMetrics&, const QskGraduationMetrics&, qreal progress );

    Q_REQUIRED_RESULT QskHashValue hash( QskHashValue seed = 0 ) const noexcept;

  private:
    qreal m_minorRatio = 0.0;
    qreal m_mediumRatio = 0.0;
    qreal m_majorRatio = 0.0;
};

inline constexpr QskGraduationMetrics::QskGraduationMetrics(
    qreal minorRatio, qreal mediumRatio, qreal majorRatio ) noexcept
    : m_minorRatio( qskConstrainedRatio( minorRatio ) )
    , m_mediumRatio( qskConstrainedRatio( mediumRatio ) )
    , m_majorRatio( qskConstrainedRatio( majorRatio ) )
{
}

inline constexpr qreal QskGraduationMetrics::majorRatio() const noexcept
{
    return m_majorRatio;
}

inline constexpr qreal QskGraduationMetrics::mediumRatio() const noexcept
{
    return m_mediumRatio;
}

inline constexpr qreal QskGraduationMetrics::minorRatio() const noexcept
{
    return m_minorRatio;
}

inline constexpr void QskGraduationMetrics::setMajorRatio( qreal ratio ) noexcept
{
    m_majorRatio = qskConstrainedRatio( ratio );
}

inline constexpr void QskGraduationMetrics::setMediumRatio( qreal ratio ) noexcept
{
    m_mediumRatio = qskConstrainedRatio( ratio );
}

inline constexpr void QskGraduationMetrics::setMinorRatio( qreal ratio ) noexcept
{
    m_minorRatio = qskConstrainedRatio( ratio );
}

inline constexpr bool QskGraduationMetrics::operator==(
    const QskGraduationMetrics& rhs ) const noexcept
{
    return qskFuzzyCompare( m_minorRatio, rhs.m_minorRatio ) &&
           qskFuzzyCompare( m_mediumRatio, rhs.m_mediumRatio ) &&
           qskFuzzyCompare( m_majorRatio, rhs.m_majorRatio );
}

inline constexpr bool QskGraduationMetrics::operator!=(
    const QskGraduationMetrics& rhs ) const noexcept
{
    return !( *this == rhs );
}

inline constexpr qreal QskGraduationMetrics::ratio( const int index ) const noexcept
{
    switch ( index % 3 )
    {
        case 0:
            return m_minorRatio;
        case 1:
            return m_mediumRatio;
        default:
            return m_majorRatio;
    }
}

inline constexpr void QskGraduationMetrics::setRatio( const int index, const qreal ratio ) noexcept
{
    switch ( index % 3 )
    {
        case 0:
            setMinorRatio( ratio );
            break;
        case 1:
            setMediumRatio( ratio );
            break;
        default:
            setMajorRatio( ratio );
            break;
    }
}

inline QskHashValue QskGraduationMetrics::hash( const QskHashValue seed ) const noexcept
{
    auto hash = qHash( m_minorRatio, seed );
    hash = qHash( m_mediumRatio, hash );
    hash = qHash( m_majorRatio, hash );
    return hash;
}

#ifndef QT_NO_DEBUG_STREAM

class QDebug;
QSK_EXPORT QDebug operator<<( QDebug, const QskGraduationMetrics& );

#endif

Q_DECLARE_TYPEINFO( QskGraduationMetrics, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( QskGraduationMetrics )

#endif