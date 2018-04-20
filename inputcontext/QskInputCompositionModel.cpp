/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskInputCompositionModel.h"
#include "QskInputContext.h"

#include <QInputMethodQueryEvent>

QskInputCompositionModel::QskInputCompositionModel(
        Attributes attributes, QskInputContext* context ):
    QObject( context ),
    m_attributes( attributes )
{
}

QskInputCompositionModel::~QskInputCompositionModel()
{
}

QskInputContext* QskInputCompositionModel::context() const
{
    return qobject_cast< QskInputContext* >( parent() );
}

void QskInputCompositionModel::composeKey( const QString& text, int spaceLeft )
{
    if ( candidateCount() > 0 )
    {
        m_preedit += text;

        requestCandidates( m_preedit );
        context()->sendText( m_preedit, false );

        return;
    }

    requestCandidates( m_preedit );

    QString txt;
    if ( candidateCount() == 0 )
    {
        txt = m_preedit.left( spaceLeft );
        spaceLeft -= txt.length();
    }
    else
    {
        txt = candidate( 0 );
        --spaceLeft;
    }

    context()->sendText( txt, true );
    m_preedit.clear();
    resetCandidates();

    if ( spaceLeft )
    {
        m_preedit = text;
        requestCandidates( m_preedit );

        if ( candidateCount() > 0 )
        {
            context()->sendText( m_preedit, false );
        }
        else
        {
            context()->sendText( m_preedit, true );
            m_preedit.clear();
            resetCandidates();
        }
    }
}

void QskInputCompositionModel::setPreeditText( const QString& text )
{
    if ( text != m_preedit )
    {
        m_preedit = text;
        requestCandidates( m_preedit );
    }
}

void QskInputCompositionModel::reset()
{
    m_preedit.clear();
    resetCandidates();
}

#include "moc_QskInputCompositionModel.cpp"
