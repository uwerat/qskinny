/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskLinearBox.h>

class QskTextLabel;

class LightDisplay : public QskLinearBox
{
    Q_OBJECT

  public:
    QSK_SUBCONTROLS( Panel, ColdPart, WarmPart, ValueText )

    LightDisplay( QQuickItem* parent = nullptr );

  protected:
    void updateLayout() override;

  private:
    QskTextLabel* m_valueLabel;
};
