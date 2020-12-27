/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_SKIN_HINT_TABLE_H
#define QSK_SKIN_HINT_TABLE_H

#include "QskAspect.h"

#include <qvariant.h>
#include <unordered_map>

class QskAnimationHint;

class QSK_EXPORT QskSkinHintTable
{
  public:
    QskSkinHintTable();
    QskSkinHintTable( const QskSkinHintTable& other );

    ~QskSkinHintTable();

    QskSkinHintTable& operator=( const QskSkinHintTable& );

    bool setAnimation( QskAspect, QskAnimationHint );
    QskAnimationHint animation( QskAspect ) const;

    bool setHint( QskAspect, const QVariant& );
    const QVariant& hint( QskAspect ) const;

    template< typename T > bool setHint( QskAspect, const T& );
    template< typename T > T hint( QskAspect ) const;

    bool removeHint( QskAspect );
    QVariant takeHint( QskAspect );

    bool hasHint( QskAspect ) const;

    const std::unordered_map< QskAspect, QVariant >& hints() const;

    bool hasAnimators() const;
    bool hasStates() const;
    bool hasHints() const;

    void clear();

    const QVariant* resolvedHint( QskAspect,
        QskAspect* resolvedAspect = nullptr ) const;

    QskAspect resolvedAspect( QskAspect ) const;

    QskAspect resolvedAnimator(
        QskAspect, QskAnimationHint& ) const;

    bool isResolutionMatching( QskAspect, QskAspect ) const;

  private:
    static const QVariant invalidHint;

    typedef std::unordered_map< QskAspect, QVariant > HintMap;
    HintMap* m_hints = nullptr;

    unsigned short m_animatorCount = 0;
    unsigned short m_statefulCount = 0;
};

inline bool QskSkinHintTable::hasHints() const
{
    return m_hints != nullptr;
}

inline bool QskSkinHintTable::hasStates() const
{
    return m_statefulCount > 0;
}

inline bool QskSkinHintTable::hasAnimators() const
{
    return m_animatorCount > 0;
}

inline bool QskSkinHintTable::hasHint( QskAspect aspect ) const
{
    if ( m_hints != nullptr )
        return m_hints->find( aspect ) != m_hints->cend();

    return false;
}

inline const QVariant& QskSkinHintTable::hint( QskAspect aspect ) const
{
    if ( m_hints != nullptr )
    {
        auto it = m_hints->find( aspect );
        if ( it != m_hints->cend() )
            return it->second;
    }

    return invalidHint;
}

template< typename T >
inline bool QskSkinHintTable::setHint( QskAspect aspect, const T& hint )
{
    return setHint( aspect, QVariant::fromValue( hint ) );
}

template< typename T >
inline T QskSkinHintTable::hint( QskAspect aspect ) const
{
    return hint( aspect ).value< T >();
}

#endif
