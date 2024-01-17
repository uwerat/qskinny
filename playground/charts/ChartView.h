/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskMainView.h>

class CircularChart;

class ChartView : public QskMainView
{
  public:
    ChartView( CircularChart*, QQuickItem* parent = nullptr );
};
