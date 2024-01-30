/******************************************************************************
 * QSkinny - Copyright (C) The authors
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
        Quit               = 1 << 0,
        RotateSkin         = 1 << 1,
        ChangeColorScheme  = 1 << 2,
        ChangeFonts        = 1 << 3,
        DebugBackground    = 1 << 4,
        DebugStatistics    = 1 << 5,

        DebugShortcuts = DebugBackground | DebugStatistics,
        AllShortcuts = Quit | RotateSkin | ChangeColorScheme
            | ChangeFonts | DebugBackground | DebugStatistics
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
