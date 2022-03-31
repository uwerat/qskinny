/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_STATE_COMBINATION_H
#define QSK_STATE_COMBINATION_H

#include "QskAspect.h"

class QSK_EXPORT QskStateCombination
{
  public:
    enum Type
    {
        Combination,
        CombinationNoState
    };

    constexpr QskStateCombination( QskAspect::State ) noexcept;
    constexpr QskStateCombination( QskAspect::States = QskAspect::States() ) noexcept;
    constexpr QskStateCombination( Type, QskAspect::States = QskAspect::States() ) noexcept;

    constexpr bool operator==( QskStateCombination ) const noexcept;
    constexpr bool operator!=( QskStateCombination ) const noexcept;

    constexpr bool isNull() const noexcept;

    void setType( Type ) noexcept;
    constexpr Type type() const noexcept;

    void setStates( QskAspect::States ) noexcept;
    void setState( QskAspect::State, bool on = true ) noexcept;
    constexpr QskAspect::States states() const noexcept;

  private:
    Type m_type;
    QskAspect::States m_states;
};

Q_DECLARE_TYPEINFO( QskStateCombination, Q_MOVABLE_TYPE );

constexpr inline QskStateCombination::QskStateCombination(
        QskAspect::State state ) noexcept
    : QskStateCombination( QskAspect::States( state ) )
{
}

constexpr inline QskStateCombination::QskStateCombination(
        QskAspect::States states ) noexcept
    : QskStateCombination( Combination, states )
{
}

constexpr inline QskStateCombination::QskStateCombination(
        Type type, QskAspect::States states ) noexcept
    : m_type( type )
    , m_states( states )
{
}

constexpr bool QskStateCombination::isNull() const noexcept
{
    return ( m_type == Combination ) && ( m_states == QskAspect::States() );
}

inline void QskStateCombination::setType( Type type ) noexcept
{
    m_type = type;
}

constexpr inline QskStateCombination::Type QskStateCombination::type() const noexcept
{
    return m_type;
}

inline void QskStateCombination::setStates( QskAspect::States states ) noexcept
{
    m_states = states;
}

inline void QskStateCombination::setState( QskAspect::State state, bool on ) noexcept
{
    if ( on )
        m_states |= state;
    else
        m_states &= ~state;
}

constexpr inline QskAspect::States QskStateCombination::states() const noexcept
{
    return m_states;
}

constexpr bool QskStateCombination::operator==( QskStateCombination other ) const noexcept
{
    return ( m_type == other.m_type ) && ( m_states == other.m_states );
}

constexpr bool QskStateCombination::operator!=( QskStateCombination other ) const noexcept
{
    return !( *this == other );
}

#endif
