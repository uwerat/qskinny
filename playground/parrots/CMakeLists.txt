############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES
    Overlay.h Overlay.cpp
    BlurringNode.h BlurringNode.cpp
    SceneTexture.h SceneTexture.cpp
    main.cpp)

qt_add_resources(SOURCES images.qrc shaders.qrc)

qsk_add_example(parrots ${SOURCES})
