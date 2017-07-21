/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#ifndef SKINNY_SHORTCUT_H_
#define SKINNY_SHORTCUT_H_

#include "SkinnyGlobal.h"
#include <QObject>

class SKINNY_EXPORT SkinnyShortcut : public QObject
{
    Q_OBJECT

public:
    enum Type
    {
        Quit = 1,
        RotateSkin = 2,
        DebugBackground = 4,
        DebugStatistics = 8,

        DebugShortcuts = DebugBackground | DebugStatistics,
        AllShortcuts = Quit | RotateSkin | DebugBackground | DebugStatistics
    };

    Q_ENUM( Type )
    Q_DECLARE_FLAGS( Types, Type )

    static void enable( Types );

private:
    SkinnyShortcut( QObject* parent = nullptr );

private Q_SLOTS:
    void rotateSkin();
    void showBackground();
    void debugStatistics();
};

Q_DECLARE_OPERATORS_FOR_FLAGS( SkinnyShortcut::Types )

#endif
