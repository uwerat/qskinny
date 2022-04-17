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

    enum TextStyle
    {
        Normal,
        Outline,
        Raised,
        Sunken
    };
    Q_ENUM_NS( TextStyle )

    enum Visibility
    {
        Visible = 1 << 0,
        Hidden  = 1 << 1
    };
    Q_ENUM_NS( Visibility )

    Q_DECLARE_FLAGS( Visibilities, Visibility )
    Q_DECLARE_OPERATORS_FOR_FLAGS( Visibilities )
}

#endif
