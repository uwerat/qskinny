/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Claus
 *****************************************************************************/

#pragma once

#include <QskLinearBox.h>

class Box : public QskLinearBox
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Panel )

    Box( const QString& title, QQuickItem* parent = nullptr );
};
