############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(target shadows)

set(SOURCES
    BoxPage.h
    BoxPage.cpp
    ShadowedBox.h
    ShadowedBox.cpp
    ArcPage.h
    ArcPage.cpp
    ShadowedArc.h
    ShadowedArc.cpp
    Slider.h
    Slider.cpp
    main.cpp
)

list(APPEND SOURCES
    ArcShadowNode.h
    ArcShadowNode.cpp
)

if (QT_VERSION_MAJOR VERSION_LESS 6) 
    qt_add_resources(SOURCES shaders.qrc)
endif()

qsk_add_example(${target} ${SOURCES})

if (QT_VERSION_MAJOR VERSION_GREATER_EQUAL 6)
    list(APPEND SHADERS
        shaders/arcshadow-vulkan.vert
        shaders/arcshadow-vulkan.frag
    )
    qsk_add_shaders( ${target} FILES ${SHADERS} OUTPUT_TARGETS shader_target)
endif()

