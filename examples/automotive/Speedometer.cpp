/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Speedometer.h"

#include <QskSkinlet.h>
#include <QskSkinnable.h>

QSK_SUBCONTROL( Speedometer, Panel )
QSK_SUBCONTROL( Speedometer, Labels )
QSK_SUBCONTROL( Speedometer, NeedleHead )
QSK_SUBCONTROL( Speedometer, Needle )

Speedometer::Speedometer( QQuickItem* parent )
    : QskRangeControl( parent )
{
}

QVector< QString > Speedometer::labels() const
{
    return m_labels;
}

void Speedometer::setLabels( const QVector< QString >& labels )
{
    m_labels = labels;
}

#include "moc_Speedometer.cpp"
