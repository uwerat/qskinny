/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef SLIDER_H
#define SLIDER_H

#include <QskSlider.h>

class Slider : public QskSlider
{
    using Inherited = QskSlider;

public:
    QSK_SUBCONTROLS( Scale, Decoration )

    Slider( QQuickItem* parent = nullptr );

    QRectF focusIndicatorRect() const override;
    QSizeF contentsSizeHint() const override;
};

#endif
