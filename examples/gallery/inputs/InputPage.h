/******************************************************************************
 * QSkinny - Copyright (C) The authors
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
