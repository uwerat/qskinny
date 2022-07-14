/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskBoundedValueInput.h>

class Dial : public QskBoundedValueInput
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Panel, TickLabels, Knob, Needle )

    Dial( QQuickItem* parent = nullptr );

    QVector< QString > tickLabels() const;
    void setTickLabels( const QVector< QString >& );

  private:
    QVector< QString > m_tickLabels;
};
