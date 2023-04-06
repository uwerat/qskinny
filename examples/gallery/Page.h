/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskLinearBox.h>

class Page : public QskLinearBox
{
  public:
    Page( QQuickItem* parent = nullptr );
    Page( Qt::Orientation, QQuickItem* parent = nullptr );
};
