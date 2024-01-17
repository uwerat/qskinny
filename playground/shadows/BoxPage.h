/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskLinearBox.h>

class QskSlider;

class BoxPage : public QskLinearBox
{
  public:
    BoxPage( QQuickItem* parent = nullptr );

  private:
    void addSlider( int row, const QString&, QskSlider* );
};
