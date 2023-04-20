/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskMainView.h>

class CircularChart;

class ChartView : public QskMainView
{
  public:
    ChartView( CircularChart*, QQuickItem* parent = nullptr );
};
