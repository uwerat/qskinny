/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_PLACEMENT_POLICY_H
#define QSK_PLACEMENT_POLICY_H

#include "QskGlobal.h"
#include "QskNamespace.h"
#include <qmetaobject.h>

class QSK_EXPORT QskPlacementPolicy
{
    Q_GADGET

    Q_PROPERTY( Policy visiblePolicy READ visiblePolicy WRITE setVisiblePolicy )
    Q_PROPERTY( Policy hiddenPolicy READ hiddenPolicy WRITE setHiddenPolicy )

  public:

    enum Policy
    {
        Ignore,
        Reserve,
        Adjust
    };
    Q_ENUM( Policy )

    constexpr QskPlacementPolicy() noexcept;
    constexpr QskPlacementPolicy( Policy ) noexcept;
    constexpr QskPlacementPolicy( Policy visiblePolicy, Policy hiddenPolicy ) noexcept;
    constexpr QskPlacementPolicy( Qsk::Visibilities, Policy ) noexcept;

    constexpr bool operator==( const QskPlacementPolicy& ) const noexcept;
    constexpr bool operator!=( const QskPlacementPolicy& ) const noexcept;

    void setPolicy( Qsk::Visibilities, Policy ) noexcept;
    constexpr Policy policy( Qsk::Visibility ) const noexcept;

    constexpr bool isEffective() const noexcept;

    constexpr bool isIgnoring( Qsk::Visibility ) const noexcept;
    constexpr bool isAdjusting( Qsk::Visibility ) const noexcept;

    void setVisiblePolicy( Policy ) noexcept;
    constexpr Policy visiblePolicy() const noexcept;

    void setHiddenPolicy( Policy ) noexcept;
    constexpr Policy hiddenPolicy() const noexcept;

  private:
    Policy m_visiblePolicy : 3;
    Policy m_hiddenPolicy : 3;
};

inline constexpr QskPlacementPolicy::QskPlacementPolicy() noexcept
    : m_visiblePolicy( Adjust )
    , m_hiddenPolicy( Ignore )
{
}

inline constexpr QskPlacementPolicy::QskPlacementPolicy(
        Policy visiblePolicy, Policy hiddenPolicy ) noexcept
    : m_visiblePolicy( visiblePolicy )
    , m_hiddenPolicy( hiddenPolicy )
{
}

constexpr QskPlacementPolicy::QskPlacementPolicy( Policy policy ) noexcept
    : m_visiblePolicy( policy )
    , m_hiddenPolicy( policy )
{
}

constexpr QskPlacementPolicy::QskPlacementPolicy(
        Qsk::Visibilities visibilities, Policy policy ) noexcept
    : m_visiblePolicy( visibilities & Qsk::Visible ? policy : Adjust )
    , m_hiddenPolicy( visibilities & Qsk::Hidden ? policy : Ignore )
{
}

inline constexpr bool QskPlacementPolicy::operator==(
    const QskPlacementPolicy& other ) const noexcept
{
    return ( other.m_visiblePolicy == m_visiblePolicy ) &&
           ( other.m_hiddenPolicy == m_hiddenPolicy );
}

inline constexpr bool QskPlacementPolicy::operator!=(
    const QskPlacementPolicy& other ) const noexcept
{
    return !( *this == other );
}

inline void QskPlacementPolicy::setPolicy(
    Qsk::Visibilities visibilities, Policy policy ) noexcept
{
    if ( visibilities & Qsk::Visible )
        m_visiblePolicy = policy;

    if ( visibilities & Qsk::Hidden )
        m_hiddenPolicy = policy;
}

inline constexpr QskPlacementPolicy::Policy
QskPlacementPolicy::policy( Qsk::Visibility visibility ) const noexcept
{
    return ( visibility == Qsk::Hidden ) ? m_hiddenPolicy : m_visiblePolicy;
}

inline void QskPlacementPolicy::setVisiblePolicy( Policy policy ) noexcept
{
    m_visiblePolicy = policy;
}

inline constexpr QskPlacementPolicy::Policy QskPlacementPolicy::visiblePolicy() const noexcept
{
    return m_visiblePolicy;
}

inline void QskPlacementPolicy::setHiddenPolicy( Policy policy ) noexcept
{
    m_hiddenPolicy = policy;
}

inline constexpr QskPlacementPolicy::Policy QskPlacementPolicy::hiddenPolicy() const noexcept
{
    return m_hiddenPolicy;
}

inline constexpr bool QskPlacementPolicy::isEffective() const noexcept
{
    return ( m_visiblePolicy != Ignore ) || ( m_hiddenPolicy != Ignore );
}

inline constexpr bool QskPlacementPolicy::isIgnoring( Qsk::Visibility visibility ) const noexcept
{
    return ( visibility == Qsk::Visible )
        ? ( m_visiblePolicy == Ignore ) : ( m_hiddenPolicy == Ignore );
}

inline constexpr bool QskPlacementPolicy::isAdjusting( Qsk::Visibility visibility ) const noexcept
{
    return ( visibility == Qsk::Visible )
        ? ( m_visiblePolicy == Adjust ) : ( m_hiddenPolicy == Adjust );
}

Q_DECLARE_TYPEINFO( QskPlacementPolicy, Q_MOVABLE_TYPE );
Q_DECLARE_METATYPE( QskPlacementPolicy )

#ifndef QT_NO_DEBUG_STREAM

    class QDebug;
    QSK_EXPORT QDebug operator<<( QDebug, QskPlacementPolicy );

#endif

#endif
