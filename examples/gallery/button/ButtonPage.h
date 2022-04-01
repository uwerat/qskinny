/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include "Page.h"

class ButtonPage : public Page
{
  public:
    ButtonPage( QQuickItem* = nullptr );

  private:
    void populate();
};
