/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include "SkinnyGlobal.h"
#include <QPainterPath>

namespace SkinnyShapeFactory
{
    Q_NAMESPACE_EXPORT( SKINNY_EXPORT )

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
