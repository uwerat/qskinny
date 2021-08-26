/******************************************************************************
 * Copyright (C) 2021 Edelhirsch Software GmbH
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include <QskGraphicProvider.h>

class GraphicProvider final : public QskGraphicProvider
{
  protected:
    const QskGraphic* loadGraphic( const QString& id ) const override;
};

