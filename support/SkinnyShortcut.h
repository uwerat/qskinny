/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#pragma once

#include "SkinnyGlobal.h"
#include <QObject>

class SKINNY_EXPORT SkinnyShortcut : public QObject
{
    Q_OBJECT

  public:
    enum Type
    {
        Quit            = 1 << 0,
        RotateSkin      = 1 << 1,
        ChangeFonts     = 1 << 2,
        DebugBackground = 1 << 3,
        DebugStatistics = 1 << 4,

        DebugShortcuts = DebugBackground | DebugStatistics,
        AllShortcuts = Quit | RotateSkin | ChangeFonts | DebugBackground | DebugStatistics
    };

    Q_ENUM( Type )
    Q_DECLARE_FLAGS( Types, Type )

    static void enable( Types );

  private:
    SkinnyShortcut( QObject* parent = nullptr );

    void showBackground();
    void debugStatistics();
};

Q_DECLARE_OPERATORS_FOR_FLAGS( SkinnyShortcut::Types )
