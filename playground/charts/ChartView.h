/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskMainView.h>

class ArcControl : public QskControl
{
public:
    QSK_SUBCONTROLS(Arc)
};


class CircularChart;

class ChartView : public QskMainView
{
  public:
    ChartView( ArcControl*, QQuickItem* parent = nullptr );
};
