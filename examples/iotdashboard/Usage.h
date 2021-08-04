/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include "Box.h"
#include <QskTextLabel.h>

class UsageSpacer : public QskTextLabel
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Text )

    UsageSpacer( QQuickItem* parent = nullptr )
        : QskTextLabel( "_____", parent )
    {
        setSubcontrolProxy( QskTextLabel::Text, Text );
    }
};

class Usage : public Box
{
  public:
    Usage( QQuickItem* parent );
};
