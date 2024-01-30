/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskSkin.h>

class Skin : public QskSkin
{
  public:
    Skin();

    void initHints() override;
};
