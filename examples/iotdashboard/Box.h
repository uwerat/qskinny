/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include "ShadowedBox.h"

class Box : public ShadowedBox
{
    Q_OBJECT

  public:
    Box( const QString& title, QQuickItem* parent = nullptr );
};
