/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_NAMESPACE_H
#define QSK_NAMESPACE_H

#include "QskGlobal.h"

namespace Qsk
{
    enum Direction
    {
        LeftToRight,
        RightToLeft,
        TopToBottom,
        BottomToTop
    };

    enum Position
    {
        Top,
        Left,
        Right,
        Bottom
    };

    enum TextStyle
    {
        Normal,
        Outline,
        Raised,
        Sunken
    };
}

#endif
