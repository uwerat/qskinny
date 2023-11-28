/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_GRADUATION_METRICS_H
#define QSK_GRADUATION_METRICS_H

#include "QskTickmarks.h"
#include "QskFunctions.h"
#include <algorithm>
#include <qmetatype.h>

class QSK_EXPORT QskGraduationMetrics
{
    Q_GADGET
    Q_PROPERTY( qreal majorTickLength READ majorTickLength WRITE setMajorTickLength )
    Q_PROPERTY( qreal mediumTickLength READ mediumTickLength WRITE setMediumTickLength )
    Q_PROPERTY( qreal minorTickLength READ minorTickLength WRITE setMinorTickLength )
    Q_PROPERTY( qreal tickWidth READ tickWidth WRITE setTickWidth )

  public:
    using TickType = QskTickmarks::TickType;

    constexpr QskGraduationMetrics() noexcept = default;

    constexpr QskGraduationMetrics( qreal minorTickLength,
        qreal mediumTickLength, qreal majorTickLength,
        qreal tickWidth = 1.0 ) noexcept;

    [[nodiscard]] constexpr bool operator==( const QskGraduationMetrics& ) const noexcept;
    [[nodiscard]] constexpr bool operator!=( const QskGraduationMetrics& ) const noexcept;

    constexpr void setTickWidth( qreal ) noexcept;
    [[nodiscard]] constexpr qreal tickWidth() const noexcept;

    constexpr void setTickLength( TickType, qreal ) noexcept;
    [[nodiscard]] constexpr qreal tickLength( TickType ) const noexcept;

    constexpr void setMinorTickLength( qreal ) noexcept;
    [[nodiscard]] constexpr qreal minorTickLength() const noexcept;

    constexpr void setMediumTickLength( qreal ) noexcept;
    [[nodiscard]] constexpr qreal mediumTickLength() const noexcept;

    constexpr void setMajorTickLength( qreal ) noexcept;
    [[nodiscard]] constexpr qreal majorTickLength() const noexcept;

    [[nodiscard]] QskGraduationMetrics interpolated(
        const QskGraduationMetrics&, qreal progress ) const noexcept;

    [[nodiscard]] static QVariant interpolate(
        const QskGraduationMetrics&, const QskGraduationMetrics&, qreal progress );

    [[nodiscard]] QskHashValue hash( QskHashValue seed = 0 ) const noexcept;

    [[nodiscard]] constexpr qreal maxLength() const noexcept;

  private:
    static inline constexpr qreal validated( qreal value )
    {
        return std::max( 0.0, value );
    }

    qreal m_tickLengths[3] = {};
    qreal m_tickWidth = 1.0;
};

inline constexpr QskGraduationMetrics::QskGraduationMetrics(
        qreal minorTickLength, qreal mediumTickLength, qreal majorTickLength,
        qreal tickWidth ) noexcept
    : m_tickLengths{ validated( minorTickLength ),
        validated( mediumTickLength ), validated( majorTickLength ) }
    , m_tickWidth( tickWidth )
{
}

inline constexpr void QskGraduationMetrics::setMajorTickLength( qreal length ) noexcept
{
    setTickLength( QskTickmarks::MajorTick, length );
}

inline constexpr qreal QskGraduationMetrics::majorTickLength() const noexcept
{
    return tickLength( QskTickmarks::MajorTick );
}

inline constexpr void QskGraduationMetrics::setMediumTickLength( qreal length ) noexcept
{
    setTickLength( QskTickmarks::MediumTick, length );
}

inline constexpr qreal QskGraduationMetrics::mediumTickLength() const noexcept
{
    return tickLength( QskTickmarks::MediumTick );
}

inline constexpr void QskGraduationMetrics::setMinorTickLength( qreal length ) noexcept
{
    setTickLength( QskTickmarks::MinorTick, length );
}

inline constexpr qreal QskGraduationMetrics::minorTickLength() const noexcept
{
    return tickLength( QskTickmarks::MinorTick );
}

inline constexpr bool QskGraduationMetrics::operator==(
    const QskGraduationMetrics& other ) const noexcept
{
    return qskFuzzyCompare( m_tickLengths[0], other.m_tickLengths[0] ) &&
           qskFuzzyCompare( m_tickLengths[1], other.m_tickLengths[1] ) &&
           qskFuzzyCompare( m_tickLengths[2], other.m_tickLengths[2] &&
           qskFuzzyCompare( m_tickWidth, other.m_tickWidth ) );
}

inline constexpr bool QskGraduationMetrics::operator!=(
    const QskGraduationMetrics& rhs ) const noexcept
{
    return !( *this == rhs );
}

inline constexpr void QskGraduationMetrics::setTickWidth( qreal width ) noexcept
{
    m_tickWidth = validated( width );
}

inline constexpr qreal QskGraduationMetrics::tickWidth() const noexcept
{
    return m_tickWidth;
}

inline constexpr void QskGraduationMetrics::setTickLength(
    TickType type, qreal length ) noexcept
{
    m_tickLengths[ type ] = validated( length );
}

inline constexpr qreal QskGraduationMetrics::tickLength(
    const QskTickmarks::TickType type ) const noexcept
{
    return m_tickLengths[ type ];
}

inline constexpr qreal QskGraduationMetrics::maxLength() const noexcept
{
    using namespace std;
    return max( max( m_tickLengths[0], m_tickLengths[1] ), m_tickLengths[2] );
}

inline QskHashValue QskGraduationMetrics::hash( const QskHashValue seed ) const noexcept
{
    auto hash = qHash( m_tickLengths[0], seed );
    hash = qHash( m_tickLengths[1], hash );
    hash = qHash( m_tickLengths[2], hash );
    hash = qHash( m_tickWidth, hash );
    return hash;
}

#ifndef QT_NO_DEBUG_STREAM

class QDebug;
QSK_EXPORT QDebug operator<<( QDebug, const QskGraduationMetrics& );

#endif

Q_DECLARE_TYPEINFO( QskGraduationMetrics, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( QskGraduationMetrics )

#endif
