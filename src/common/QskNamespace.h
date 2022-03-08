/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_NAMESPACE_H
#define QSK_NAMESPACE_H

#include "QskGlobal.h"
#include <qmetaobject.h>

namespace Qsk
{
    QSK_EXPORT Q_NAMESPACE

    enum Direction
    {
        LeftToRight,
        RightToLeft,
        TopToBottom,
        BottomToTop
    };
    Q_ENUM_NS( Direction )

    enum Position
    {
        Top,
        Left,
        Right,
        Bottom
    };
    Q_ENUM_NS( Position )

    enum TextStyle
    {
        Normal,
        Outline,
        Raised,
        Sunken
    };
    Q_ENUM_NS( TextStyle )
}

#endif
