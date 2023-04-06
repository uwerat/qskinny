/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
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
