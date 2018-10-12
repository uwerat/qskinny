/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef SKINNY_SHAPE_FACTORY_H_
#define SKINNY_SHAPE_FACTORY_H_

#include "SkinnyGlobal.h"
#include <QPainterPath>

namespace SkinnyShapeFactory
{
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

        ShapeCount
    };

    SKINNY_EXPORT QPainterPath shapePath( Shape, const QSizeF& );
}

#endif
