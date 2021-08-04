/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include "Page.h"

class SliderPage : public Page
{
  public:
    SliderPage( QQuickItem* = nullptr );

  private:
    void populate();
};
