/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskSlider.h>

class CustomSlider : public QskSlider
{
    using Inherited = QskSlider;

  public:
    QSK_SUBCONTROLS( Scale, Decoration )

    CustomSlider( QQuickItem* parent = nullptr );

    QRectF focusIndicatorRect() const override;
};
