/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef SPEEDOMETERDISPLAY_H
#define SPEEDOMETERDISPLAY_H

#include <QskLinearBox.h>

class SpeedometerDisplay : public QskLinearBox
{
  public:
    SpeedometerDisplay( QQuickItem* parent = nullptr );
};

#endif
