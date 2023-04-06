/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskSlider.h>

class OtherSlider : public QskSlider
{
  public:
    // Slider overriding many hints from the skin.
    OtherSlider( QQuickItem* = nullptr );
};
