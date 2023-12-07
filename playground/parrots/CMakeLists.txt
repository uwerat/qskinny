############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES
    BlurredOverlay.h BlurredOverlay.cpp
    BlurredTextureNode.h BlurredTextureNode.cpp
    main.cpp)

qt_add_resources(SOURCES images.qrc shaders.qrc)

qsk_add_example(parrots ${SOURCES})
