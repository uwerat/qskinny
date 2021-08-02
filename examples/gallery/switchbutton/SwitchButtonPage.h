/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef SWITCH_BUTTON_PAGE_H
#define SWITCH_BUTTON_PAGE_H

#include "Page.h"

class SwitchButtonPage : public Page
{
  public:
    SwitchButtonPage( QQuickItem* = nullptr );

  private:
    void populate();
};

#endif
