############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES
    QskWindowsGlobal.h QskWindowsSkin.h QskWindowsSkin.cpp
    QskWindowsSkinFactory.h QskWindowsSkinFactory.cpp
)
qt_add_resources(SOURCES icons.qrc)

qsk_add_plugin(windowsskin skins QskWindowsSkinFactory ${SOURCES})
set_target_properties(windowsskin PROPERTIES DEFINE_SYMBOL QSK_Windows_MAKEDLL )
