/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_LAYOUT_HINT_H
#define QSK_LAYOUT_HINT_H

#include "QskGlobal.h"
#include <qglobal.h>
#include <qmetatype.h>
#include <limits>

class QSK_EXPORT QskLayoutHint
{
    Q_GADGET

    Q_PROPERTY( qreal minimum READ minimum WRITE setMinimum )
    Q_PROPERTY( qreal preferred READ preferred WRITE setPreferred )
    Q_PROPERTY( qreal maximum READ maximum WRITE setMaximum )

  public:
    constexpr QskLayoutHint() noexcept = default;
    constexpr QskLayoutHint( qreal minimum, qreal preferred, qreal maximum ) noexcept;

    void normalize() noexcept;

    QskLayoutHint normalized() const noexcept;
    constexpr bool isDefault() const noexcept;

    qreal size( int which ) const noexcept;
    void setSize( int which, qreal size ) noexcept;

    constexpr bool operator==( const QskLayoutHint& ) const noexcept;
    constexpr bool operator!=( const QskLayoutHint& ) const noexcept;

    void setMinimum( qreal value ) noexcept;
    constexpr qreal minimum() const noexcept;

    void setPreferred( qreal value ) noexcept;
    constexpr qreal preferred() const noexcept;

    void setMaximum( qreal value ) noexcept;
    constexpr qreal maximum() const noexcept;

    void setSizes( qreal minimum, qreal preferred, qreal maximum ) noexcept;
    void expandTo( const QskLayoutHint& ) noexcept;

    void expandMinimum( qreal value ) noexcept;
    void expandPreferred( qreal value ) noexcept;
    void expandMaximum( qreal value ) noexcept;

    static qreal combined( int which, qreal value1, qreal value2 ) noexcept;
    static constexpr qreal unlimited = std::numeric_limits< float >::max();

  private:
    qreal m_minimum = 0.0;
    qreal m_preferred = 0.0;
    qreal m_maximum = QskLayoutHint::unlimited;
};

Q_DECLARE_TYPEINFO( QskLayoutHint, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( QskLayoutHint )

QT_WARNING_PUSH
QT_WARNING_DISABLE_CLANG("-Wfloat-equal")
QT_WARNING_DISABLE_GCC("-Wfloat-equal")
QT_WARNING_DISABLE_INTEL(1572)

inline constexpr QskLayoutHint::QskLayoutHint(
        qreal minimum, qreal preferred, qreal maximum ) noexcept
    : m_minimum( minimum )
    , m_preferred( preferred )
    , m_maximum( maximum )
{
}

inline constexpr qreal QskLayoutHint::minimum() const noexcept
{
    return m_minimum;
}

inline void QskLayoutHint::setMinimum( qreal value ) noexcept
{
    m_minimum = value;
}

inline constexpr qreal QskLayoutHint::preferred() const noexcept
{
    return m_preferred;
}

inline void QskLayoutHint::setPreferred( qreal value ) noexcept
{
    m_preferred = value;
}

inline constexpr qreal QskLayoutHint::maximum() const noexcept
{
    return m_maximum;
}

inline void QskLayoutHint::setMaximum( qreal value ) noexcept
{
    m_maximum = value;
}

inline void QskLayoutHint::expandMinimum( qreal value ) noexcept
{
    if ( value > m_minimum )
        m_minimum = value;
}

inline void QskLayoutHint::expandPreferred( qreal value ) noexcept
{
    if ( value > m_preferred )
        m_preferred = value;
}

inline void QskLayoutHint::expandMaximum( qreal value ) noexcept
{
    if ( value > m_maximum )
        m_maximum = value;
}

inline void QskLayoutHint::setSizes(
    qreal minimum, qreal preferred, qreal maximum ) noexcept
{
    m_minimum = minimum;
    m_preferred = preferred;
    m_maximum = maximum;
}

inline qreal QskLayoutHint::size( int which ) const noexcept
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

inline constexpr bool QskLayoutHint::isDefault() const noexcept
{
    return ( m_minimum == 0.0 ) && ( m_preferred == 0.0 )
        && ( m_maximum == QskLayoutHint::unlimited );
}

inline constexpr bool QskLayoutHint::operator==( const QskLayoutHint& other ) const noexcept
{
    return ( m_preferred == other.m_preferred )
        && ( m_minimum == other.m_minimum )
        && ( m_maximum == other.m_maximum );
}

inline constexpr bool QskLayoutHint::operator!=( const QskLayoutHint& other ) const noexcept
{
    return !( *this == other );
}

inline QskLayoutHint QskLayoutHint::normalized() const noexcept
{
    auto hint = *this;
    hint.normalize();
    return hint;
}

QT_WARNING_POP

#ifndef QT_NO_DEBUG_STREAM

class QDebug;
QSK_EXPORT QDebug operator<<( QDebug, const QskLayoutHint& );

#endif

#endif
