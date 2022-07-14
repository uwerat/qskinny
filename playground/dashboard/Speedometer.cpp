/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Speedometer.h"

#include <QskSkinlet.h>
#include <QskSkinnable.h>

QSK_SUBCONTROL( Speedometer, Panel )
QSK_SUBCONTROL( Speedometer, TickLabels )
QSK_SUBCONTROL( Speedometer, Knob )
QSK_SUBCONTROL( Speedometer, Needle )

Speedometer::Speedometer( QQuickItem* parent )
    : QskBoundedValueInput( parent )
{
}

QVector< QString > Speedometer::tickLabels() const
{
    return m_tickLabels;
}

void Speedometer::setTickLabels( const QVector< QString >& labels )
{
    m_tickLabels = labels;
}

#include "moc_Speedometer.cpp"
