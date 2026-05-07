/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskLinearBox.h>

class TextLabel;
class OptionsPanel;

class TextView : public QskLinearBox
{
  public:
    TextView( QQuickItem* parent = nullptr );

  private:
    void updateLabel();

    OptionsPanel* m_panel;
    TextLabel* m_label;
};
