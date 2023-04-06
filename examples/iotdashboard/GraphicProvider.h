/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 *           SPDX-License-Identifier: BSD-3-Claus
 *****************************************************************************/

#pragma once

#include <QskGraphicProvider.h>

class GraphicProvider final : public QskGraphicProvider
{
  protected:
    const QskGraphic* loadGraphic( const QString& id ) const override;
};

