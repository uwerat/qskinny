/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskLinearBox.h>
#include <QskPushButton.h>

class RoundButton : QskPushButton
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Panel )
    QSK_STATES( Top )

    RoundButton( QskAspect::Placement placement, QQuickItem* parent );
};

class UpAndDownButton : public QskLinearBox
{
    Q_OBJECT

  public:
    UpAndDownButton( QQuickItem* parent );
};
