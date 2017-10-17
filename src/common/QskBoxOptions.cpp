/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxOptions.h"

bool QskBoxOptions::isVisible() const
{
    if ( fillGradient.isVisible() )
        return true;

    return !border.isNull() && borderColors.isVisible();
}
