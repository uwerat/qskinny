/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Claus
 *****************************************************************************/

#pragma once

#include <QskGridBox.h>
#include <QskLinearBox.h>

class MainContentGridBox : public QskGridBox
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Panel )

    MainContentGridBox( QQuickItem* parent = nullptr )
        : QskGridBox( parent )
    {
        setSubcontrolProxy( QskGridBox::Panel, Panel );
    }
};
