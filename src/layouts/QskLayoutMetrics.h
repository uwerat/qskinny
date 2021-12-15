/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_LAYOUT_METRICS_H
#define QSK_LAYOUT_METRICS_H

#include "QskGlobal.h"
#include <qglobal.h>
#include <qmetatype.h>
#include <limits>

class QSK_EXPORT QskLayoutMetrics
{
    Q_GADGET

    Q_PROPERTY( qreal minimum READ minimum WRITE setMinimum )
    Q_PROPERTY( qreal preferred READ preferred WRITE setPreferred )
    Q_PROPERTY( qreal maximum READ maximum WRITE setMaximum )

  public:
    constexpr QskLayoutMetrics() noexcept = default;
    constexpr QskLayoutMetrics( qreal minimum, qreal preferred, qreal maximum ) noexcept;

    void normalize() noexcept;

    QskLayoutMetrics normalized() const noexcept;
    constexpr bool isDefault() const noexcept;

    qreal metric( int which ) const noexcept;
    void setMetric( int which, qreal metric ) noexcept;

    constexpr bool operator==( const QskLayoutMetrics& ) const noexcept;
    constexpr bool operator!=( const QskLayoutMetrics& ) const noexcept;

    void setMinimum( qreal value ) noexcept;
    constexpr qreal minimum() const noexcept;

    void setPreferred( qreal value ) noexcept;
    constexpr qreal preferred() const noexcept;

    void setMaximum( qreal value ) noexcept;
    constexpr qreal maximum() const noexcept;

    void setMetrics( qreal minimum, qreal preferred, qreal maximum ) noexcept;
    void expandTo( const QskLayoutMetrics& ) noexcept;

    void expandMinimum( qreal value ) noexcept;
    void expandPreferred( qreal value ) noexcept;
    void expandMaximum( qreal value ) noexcept;

    static qreal combined( int which, qreal value1, qreal value2 ) noexcept;
    static constexpr qreal unlimited = std::numeric_limits< float >::max();

  private:
    qreal m_minimum = 0.0;
    qreal m_preferred = 0.0;
    qreal m_maximum = QskLayoutMetrics::unlimited;
};

Q_DECLARE_TYPEINFO( QskLayoutMetrics, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( QskLayoutMetrics )

QT_WARNING_PUSH
QT_WARNING_DISABLE_CLANG("-Wfloat-equal")
QT_WARNING_DISABLE_GCC("-Wfloat-equal")
QT_WARNING_DISABLE_INTEL(1572)

inline constexpr QskLayoutMetrics::QskLayoutMetrics(
        qreal minimum, qreal preferred, qreal maximum ) noexcept
    : m_minimum( minimum )
    , m_preferred( preferred )
    , m_maximum( maximum )
{
}

inline constexpr qreal QskLayoutMetrics::minimum() const noexcept
{
    return m_minimum;
}

inline void QskLayoutMetrics::setMinimum( qreal value ) noexcept
{
    m_minimum = value;
}

inline constexpr qreal QskLayoutMetrics::preferred() const noexcept
{
    return m_preferred;
}

inline void QskLayoutMetrics::setPreferred( qreal value ) noexcept
{
    m_preferred = value;
}

inline constexpr qreal QskLayoutMetrics::maximum() const noexcept
{
    return m_maximum;
}

inline void QskLayoutMetrics::setMaximum( qreal value ) noexcept
{
    m_maximum = value;
}

inline void QskLayoutMetrics::expandMinimum( qreal value ) noexcept
{
    if ( value > m_minimum )
        m_minimum = value;
}

inline void QskLayoutMetrics::expandPreferred( qreal value ) noexcept
{
    if ( value > m_preferred )
        m_preferred = value;
}

inline void QskLayoutMetrics::expandMaximum( qreal value ) noexcept
{
    if ( value > m_maximum )
        m_maximum = value;
}

inline void QskLayoutMetrics::setMetrics(
    qreal minimum, qreal preferred, qreal maximum ) noexcept
{
    m_minimum = minimum;
    m_preferred = preferred;
    m_maximum = maximum;
}

inline qreal QskLayoutMetrics::metric( int which ) const noexcept
{
    switch( which )
    {
        case Qt::MinimumSize:
            return m_minimum;

        case Qt::PreferredSize:
            return m_preferred;

        case Qt::MaximumSize:
            return m_maximum;
    }

    return 0.0;
}

inline constexpr bool QskLayoutMetrics::isDefault() const noexcept
{
    return ( m_minimum == 0.0 ) && ( m_preferred == 0.0 )
        && ( m_maximum == QskLayoutMetrics::unlimited );
}

inline constexpr bool QskLayoutMetrics::operator==(
    const QskLayoutMetrics& other ) const noexcept
{
    return ( m_preferred == other.m_preferred )
        && ( m_minimum == other.m_minimum )
        && ( m_maximum == other.m_maximum );
}

inline constexpr bool QskLayoutMetrics::operator!=(
    const QskLayoutMetrics& other ) const noexcept
{
    return !( *this == other );
}

inline QskLayoutMetrics QskLayoutMetrics::normalized() const noexcept
{
    auto hint = *this;
    hint.normalize();
    return hint;
}

QT_WARNING_POP

#ifndef QT_NO_DEBUG_STREAM

class QDebug;
QSK_EXPORT QDebug operator<<( QDebug, const QskLayoutMetrics& );

#endif

#endif
