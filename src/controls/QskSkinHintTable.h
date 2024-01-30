/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SKIN_HINT_TABLE_H
#define QSK_SKIN_HINT_TABLE_H

#include "QskAspect.h"

#include <qvariant.h>
#include <qhash.h>

class QskAnimationHint;

class QSK_EXPORT QskSkinHintTable
{
  public:
    QskSkinHintTable();
    QskSkinHintTable( const QskSkinHintTable& );
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

    const QHash< QskAspect, QVariant >& hints() const;

    bool hasAnimators() const;
    bool hasHints() const;

    QskAspect::States states() const;

    void clear();

    const QVariant* resolvedHint( QskAspect,
        QskAspect* resolvedAspect = nullptr ) const;

    QskAspect resolvedAspect( QskAspect ) const;

    QskAspect resolvedAnimator(
        QskAspect, QskAnimationHint& ) const;

    bool isResolutionMatching( QskAspect, QskAspect ) const;

  private:

    static const QVariant invalidHint;

    QHash< QskAspect, QVariant >* m_hints = nullptr;

    unsigned short m_animatorCount = 0;
    QskAspect::States m_states;
};

inline bool QskSkinHintTable::hasHints() const
{
    return m_hints != nullptr;
}

inline QskAspect::States QskSkinHintTable::states() const
{
    return m_states;
}

inline bool QskSkinHintTable::hasAnimators() const
{
    return m_animatorCount > 0;
}

inline bool QskSkinHintTable::hasHint( QskAspect aspect ) const
{
    return m_hints && m_hints->contains( aspect );
}

inline const QVariant& QskSkinHintTable::hint( QskAspect aspect ) const
{
    if ( m_hints != nullptr )
    {
        auto it = m_hints->constFind( aspect );
        if ( it != m_hints->constEnd() )
            return it.value();
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
