/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include "Box.h"

class QskTextLabel;

class BoxWithButtons : public Box
{
  public:
    QSK_SUBCONTROLS( Panel, ValuePanel, ValueText )

    BoxWithButtons( const QString& title, const QString& prefix,
                    const int initialValue, const QString& suffix,
                    bool isBright, QQuickItem* parent = nullptr );

  private:
    void setValue( const int value );

    const QString m_prefix;
    int m_value;
    const QString m_suffix;
    QskTextLabel* m_valueLabel;
};
