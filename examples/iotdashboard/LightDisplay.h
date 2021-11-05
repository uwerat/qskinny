/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskBoundedControl.h>

class LightDisplay : public QskBoundedControl
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Groove, ColdPart, WarmPart, ValueText, LeftLabel, RightLabel )

    LightDisplay( QQuickItem* parent = nullptr );
};
