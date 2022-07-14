/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
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
