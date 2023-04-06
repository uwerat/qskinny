/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Claus
 *****************************************************************************/

#pragma once

#include <QskPushButton.h>
#include <Qt>

class RoundButton : public QskPushButton
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Panel )
    QSK_STATES( Top )

    RoundButton( Qt::Edge, QQuickItem* parent );
};
