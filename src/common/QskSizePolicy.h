/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SIZE_POLICY_H
#define QSK_SIZE_POLICY_H

#include "QskGlobal.h"
#include <qnamespace.h>
#include <qmetatype.h>

class QSK_EXPORT QskSizePolicy
{
    Q_GADGET

    Q_PROPERTY( Policy horizontalPolicy
        READ horizontalPolicy WRITE setHorizontalPolicy )

    Q_PROPERTY( Policy verticalPolicy
        READ verticalPolicy WRITE setVerticalPolicy )

  public:
    enum Flag
    {
        GrowFlag        = 1 << 0,
        ExpandFlag      = 1 << 1,
        ShrinkFlag      = 1 << 2,
        IgnoreFlag      = 1 << 3,
        ConstrainedFlag = 1 << 4
    };

    enum Policy
    {
        Fixed = 0,

        Minimum          = GrowFlag,
        Maximum          = ShrinkFlag,
        Preferred        = GrowFlag | ShrinkFlag,
        MinimumExpanding = GrowFlag | ExpandFlag,
        Expanding        = GrowFlag | ShrinkFlag | ExpandFlag,

        Ignored          = ShrinkFlag | GrowFlag | IgnoreFlag,

        Constrained                 = ConstrainedFlag,

        ConstrainedMinimum          = ConstrainedFlag | Minimum,
        ConstrainedMaximum          = ConstrainedFlag | Maximum,
        ConstrainedPreferred        = ConstrainedFlag | Preferred,
        ConstrainedMinimumExpanding = ConstrainedFlag | MinimumExpanding,
        ConstrainedExpanding        = ConstrainedFlag | Expanding
    };

    enum ConstraintType
    {
        Unconstrained  = 0,

        WidthForHeight = 1 << 0,
        HeightForWidth = 1 << 1
    };

    Q_ENUM( Flag )
    Q_ENUM( Policy )
    Q_ENUM( ConstraintType )

    constexpr QskSizePolicy() noexcept;
    constexpr QskSizePolicy( Policy horizontalPolicy, Policy verticalPolicy ) noexcept;

    constexpr bool operator==( const QskSizePolicy& ) const noexcept;
    constexpr bool operator!=( const QskSizePolicy& ) const noexcept;

    void setHorizontalPolicy( Policy ) noexcept;
    constexpr Policy horizontalPolicy() const noexcept;

    void setVerticalPolicy( Policy ) noexcept;
    constexpr Policy verticalPolicy() const noexcept;

    void setPolicy( Qt::Orientation, Policy ) noexcept;
    constexpr Policy policy( Qt::Orientation ) const noexcept;

    ConstraintType constraintType() const noexcept;
    constexpr bool isConstrained( Qt::Orientation ) const noexcept;

    Qt::SizeHint effectiveSizeHintType(
        Qt::SizeHint, Qt::Orientation ) const noexcept;

    constexpr QskSizePolicy transposed() const noexcept;
    void transpose() noexcept;

  private:
    unsigned char m_horizontalPolicy;
    unsigned char m_verticalPolicy;
};

inline constexpr QskSizePolicy::QskSizePolicy() noexcept
    : m_horizontalPolicy( Ignored )
    , m_verticalPolicy( Ignored )
{
}

inline constexpr QskSizePolicy::QskSizePolicy(
        Policy horizontalPolicy, Policy verticalPolicy ) noexcept
    : m_horizontalPolicy( horizontalPolicy )
    , m_verticalPolicy( verticalPolicy )
{
}

inline constexpr bool QskSizePolicy::operator==(
    const QskSizePolicy& other ) const noexcept
{
    return ( other.m_horizontalPolicy == m_horizontalPolicy ) &&
           ( other.m_verticalPolicy == m_verticalPolicy );
}

inline constexpr bool QskSizePolicy::operator!=(
    const QskSizePolicy& other ) const noexcept
{
    return !( *this == other );
}

inline void QskSizePolicy::setHorizontalPolicy( Policy policy ) noexcept
{
    m_horizontalPolicy = static_cast< unsigned char >( policy );
}

inline constexpr QskSizePolicy::Policy QskSizePolicy::horizontalPolicy() const noexcept
{
    return static_cast< Policy >( m_horizontalPolicy );
}

inline void QskSizePolicy::setVerticalPolicy( Policy policy ) noexcept
{
    m_verticalPolicy = static_cast< unsigned char >( policy );
}

inline constexpr QskSizePolicy::Policy QskSizePolicy::verticalPolicy() const noexcept
{
    return static_cast< Policy >( m_verticalPolicy );
}

inline constexpr QskSizePolicy::Policy QskSizePolicy::policy(
    Qt::Orientation orientation ) const noexcept
{
    return static_cast< Policy >(
        ( orientation == Qt::Horizontal ) ? m_horizontalPolicy : m_verticalPolicy );
}

inline void QskSizePolicy::setPolicy(
    Qt::Orientation orientation, Policy policy ) noexcept
{
    if ( orientation == Qt::Horizontal )
        m_horizontalPolicy = static_cast< unsigned char >( policy );
    else
        m_verticalPolicy = static_cast< unsigned char >( policy );
}

inline constexpr bool QskSizePolicy::isConstrained(
    Qt::Orientation orientation ) const noexcept
{
    return ( policy( orientation ) & ConstrainedFlag );
}

inline constexpr QskSizePolicy QskSizePolicy::transposed() const noexcept
{
    return QskSizePolicy( verticalPolicy(), horizontalPolicy() );
}

#ifndef QT_NO_DEBUG_STREAM
class QDebug;
QSK_EXPORT QDebug operator<<( QDebug, const QskSizePolicy& );
#endif

Q_DECLARE_TYPEINFO( QskSizePolicy, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( QskSizePolicy )

#endif
