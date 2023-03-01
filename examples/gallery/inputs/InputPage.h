/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include "Page.h"

class InputPage : public Page
{
  public:
    InputPage( QQuickItem* = nullptr );

  private:
    void syncValues( qreal );
};
