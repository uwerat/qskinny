/******************************************************************************
 * Copyright (C) 2022 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <QVector>
#include <QskLinearBox.h>

class StoragePage final : public QskLinearBox
{
  public:
    QSK_SUBCONTROLS( Panel, Status )

    StoragePage( QQuickItem* parent = nullptr );

  private:
    void addRow( const QString& title, const QString& description,
        qreal pictures, qreal music, qreal videos, qreal documents, qreal others );
};
