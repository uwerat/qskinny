############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES
    Overlay.h Overlay.cpp
    TextureFilterMaterial.h TextureFilterMaterial.cpp
    TextureFilterNode.h TextureFilterNode.cpp
    main.cpp)

qt_add_resources(SOURCES images.qrc shaders.qrc)

qsk_add_example(parrots ${SOURCES})
