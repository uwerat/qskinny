/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskLinearBox.h>

class Page : public QskLinearBox
{
  public:
    Page( QQuickItem* parent = nullptr );
    Page( Qt::Orientation, QQuickItem* parent = nullptr );
};
