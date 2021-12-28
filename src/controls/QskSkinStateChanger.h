/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
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
    if ( m_oldStates != m_skinnable->skinStates() )
        m_skinnable->replaceSkinStates( m_oldStates );
}

inline void QskSkinStateChanger::setStates( QskAspect::States states )
{
    if ( states != m_skinnable->skinStates() )
        m_skinnable->replaceSkinStates( states );
}

#endif
