/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#ifndef QSK_SKIN_STATE_CHANGER_H
#define QSK_SKIN_STATE_CHANGER_H

#include "QskAspect.h"
#include "QskSkinnable.h"

class QskSkinStateChanger
{
  public:
    QskSkinStateChanger( const QskSkinnable* );
    ~QskSkinStateChanger();

    void setStates( QskAspect::States, int sampleIndex = -1 );
    void resetStates();

  private:
    QskSkinnable* m_skinnable;
    const QskAspect::States m_oldStates;
};

inline QskSkinStateChanger::QskSkinStateChanger( const QskSkinnable* skinnable )
    : m_skinnable( const_cast< QskSkinnable* >( skinnable ) )
    , m_oldStates( skinnable->skinStates() )
{
}

inline QskSkinStateChanger::~QskSkinStateChanger()
{
    resetStates();
}

inline void QskSkinStateChanger::setStates(
    QskAspect::States states, int sampleIndex )
{
    m_skinnable->replaceSkinStates( states, sampleIndex );
}

inline void QskSkinStateChanger::resetStates()
{
    m_skinnable->replaceSkinStates( m_oldStates, -1 );
}

#endif
