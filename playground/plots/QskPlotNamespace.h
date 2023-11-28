/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include <qmetaobject.h>

namespace QskPlot
{
    Q_NAMESPACE

    // for the moment only 2 axes
    enum Axis
    {
        XBottom = 0,
        YLeft   = 1
    };
    Q_ENUM_NS( Axis )
}
