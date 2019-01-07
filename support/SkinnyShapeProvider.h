/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef SKINNY_SHAPE_PROVIDER_H
#define SKINNY_SHAPE_PROVIDER_H

#include "SkinnyGlobal.h"
#include <QskGraphicProvider.h>

class SKINNY_EXPORT SkinnyShapeProvider : public QskGraphicProvider
{
  protected:
    const QskGraphic* loadGraphic( const QString& id ) const override final;
};

#endif

