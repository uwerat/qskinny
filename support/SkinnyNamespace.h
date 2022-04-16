/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include "SkinnyGlobal.h"
#include <QskAnimationHint.h>

namespace Skinny
{
    SKINNY_EXPORT void changeSkin( QskAnimationHint hint = 500 );
    SKINNY_EXPORT void changeFonts( int increment );
    SKINNY_EXPORT void init();
}
