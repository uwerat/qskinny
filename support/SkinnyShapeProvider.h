/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include "SkinnyGlobal.h"
#include <QskGraphicProvider.h>

class SKINNY_EXPORT SkinnyShapeProvider : public QskGraphicProvider
{
  protected:
    const QskGraphic* loadGraphic( const QString& id ) const override final;
};

