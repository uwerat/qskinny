/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Dial.h"

#include <QskSkinlet.h>
#include <QskSkinnable.h>

QSK_SUBCONTROL( Dial, Panel )
QSK_SUBCONTROL( Dial, TickLabels )
QSK_SUBCONTROL( Dial, Knob )
QSK_SUBCONTROL( Dial, Needle )

Dial::Dial( QQuickItem* parent )
    : QskBoundedValueInput( parent )
{
}

QVector< QString > Dial::tickLabels() const
{
    return m_tickLabels;
}

void Dial::setTickLabels( const QVector< QString >& labels )
{
    m_tickLabels = labels;
}

#include "moc_Dial.cpp"
