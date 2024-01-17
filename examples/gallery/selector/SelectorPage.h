/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include "Page.h"

class SelectorPage : public Page
{
  public:
    SelectorPage( QQuickItem* = nullptr );

  private:
    void populate();
};
