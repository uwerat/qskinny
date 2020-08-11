/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef OTHER_SLIDER_H
#define OTHER_SLIDER_H

#include <QskSlider.h>

class OtherSlider : public QskSlider
{
  public:
    // Slider overriding many hints from the skin.

    OtherSlider( QQuickItem* = nullptr );
};

#endif
