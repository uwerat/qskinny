/******************************************************************************
 * Copyright (C) 2022 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include "CircularProgressBar.h"
#include <QskControl.h>

class StorageMeter final : public CircularProgressBar
{
  public:
    QSK_SUBCONTROLS( Status )
    explicit StorageMeter( QQuickItem* parent = nullptr ) noexcept;
  public Q_SLOTS:
    void setValue( qreal value );

  private:
    QSizeF contentsSizeHint( Qt::SizeHint which, const QSizeF& constraint ) const override;
    class QskTextLabel* label = nullptr;
};
