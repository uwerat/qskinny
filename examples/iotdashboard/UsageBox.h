/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Claus
 *****************************************************************************/

#pragma once

#include "Box.h"

class UsageBox : public Box
{
  public:
    QSK_SUBCONTROLS( Separator )

    UsageBox( QQuickItem* parent = nullptr );
};
