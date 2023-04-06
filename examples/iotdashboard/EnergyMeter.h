/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Claus
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
