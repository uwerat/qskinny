/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
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

    void setStates( QskAspect::States );
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

inline void QskSkinStateChanger::setStates( QskAspect::States states )
{
    if ( states != m_skinnable->skinStates() )
        m_skinnable->replaceSkinStates( states );
}

inline void QskSkinStateChanger::resetStates()
{
    if ( m_oldStates != m_skinnable->skinStates() )
        m_skinnable->replaceSkinStates( m_oldStates );
}

#endif
