/******************************************************************************
 * Copyright (C) 2023 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include "Page.h"

class SpinBoxPage : public Page
{
  public:
    SpinBoxPage( QQuickItem* = nullptr );

  private:
    void populate();
};
