/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskControl.h>

class EnergyMeter : public QskControl
{
  public:
    EnergyMeter( const QColor&, const QskGradient&,
        int progress, QQuickItem* parent = nullptr );

  protected:
    QSizeF contentsSizeHint( Qt::SizeHint, const QSizeF& ) const override;
};
