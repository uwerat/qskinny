/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef PROGRESS_BAR_PAGE_H
#define PROGRESS_BAR_PAGE_H

#include "Page.h"

class ProgressBarPage : public Page
{
  public:
    ProgressBarPage( QQuickItem* = nullptr );

  private:
    void populate();
};

#endif
