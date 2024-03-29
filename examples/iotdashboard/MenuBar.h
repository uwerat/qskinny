/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskLinearBox.h>
#include "MainItem.h"

class QskPushButton;

class MenuBar final : public QskLinearBox
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Panel )

    MenuBar( QQuickItem* parent );

  Q_SIGNALS:
    void pageChangeRequested( const int index );

  public Q_SLOTS:
    void setActivePage( const int index );

  private:
    void populate();

    QskPushButton* m_buttons[ Cube::NumPositions ];
    uint m_currentIndex;
};
