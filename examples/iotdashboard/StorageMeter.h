/******************************************************************************
 * Copyright (C) 2022 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QskProgressRing.h>

class QskTextLabel;

class StorageMeter final : public QskProgressRing
{
  public:
    QSK_SUBCONTROLS( Status )

    StorageMeter( QQuickItem* parent = nullptr ) noexcept;

  private:
    void updateMeter( qreal value );
    QSizeF contentsSizeHint( Qt::SizeHint, const QSizeF& ) const override;

    QskTextLabel* m_label = nullptr;
};
