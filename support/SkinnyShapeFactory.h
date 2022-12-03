/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

#include "SkinnyGlobal.h"
#include <QPainterPath>

namespace SkinnyShapeFactory
{
    SKINNY_EXPORT Q_NAMESPACE

    // a couple of standard painter paths, that can
    // be used for testing

    enum Shape
    {
        Rectangle,
        Diamond,
        TriangleDown,
        TriangleUp,
        TriangleLeft,
        TriangleRight,
        Ellipse,
        Ring,
        Star,
        Hexagon,
        Arc,

        ShapeCount
    };

    SKINNY_EXPORT QPainterPath shapePath( Shape, const QSizeF& );
}
