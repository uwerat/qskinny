############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES
    Overlay.h Overlay.cpp
    TextureFilterMaterial.h TextureFilterMaterial.cpp
    TextureFilterNode.h TextureFilterNode.cpp
    main.cpp)

qt_add_resources(SOURCES images.qrc)

if (QT_VERSION_MAJOR VERSION_LESS 6)
    qt_add_resources(SOURCES shaders.qrc)
endif()

qsk_add_example(parrots ${SOURCES})

if (QT_VERSION_MAJOR VERSION_GREATER_EQUAL 6)

    qt6_add_shaders(parrots "shaders"

        BATCHABLE
        PRECOMPILE

        QUIET

        PREFIX
            "/shaders"

        FILES
            shaders/blur-vulkan.vert
            shaders/blur-vulkan.frag
            shaders/rgbswap-vulkan.frag

        OUTPUTS
            blur.vert.qsb
            blur.frag.qsb
            rgbswap.vert.qsb
    )

endif()

