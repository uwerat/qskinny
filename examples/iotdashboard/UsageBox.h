/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include "Box.h"

class UsageBox : public Box
{
  public:
    QSK_SUBCONTROLS( Separator )

    UsageBox( QQuickItem* parent = nullptr );
};
