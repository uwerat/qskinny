/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
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
    setReadOnly( true );
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
