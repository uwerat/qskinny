/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include "SkinnyGlobal.h"
#include <QskAnimationHint.h>

namespace Skinny
{
    SKINNY_EXPORT void changeSkin( QskAnimationHint hint = 500 );
    SKINNY_EXPORT void setSkin( int index, QskAnimationHint hint = 500 );
    SKINNY_EXPORT void changeFonts( int increment );
    SKINNY_EXPORT void init();
}
