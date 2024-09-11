############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES
    SkinnyGlobal.h
    SkinnyNamespace.h SkinnyNamespace.cpp
    SkinnyShapeFactory.h SkinnyShapeFactory.cpp
    SkinnyShapeProvider.h SkinnyShapeProvider.cpp
    SkinnyShortcut.h SkinnyShortcut.cpp
)

qt_add_resources(SOURCES DejaVuSans.qrc Roboto.qrc SegoeUI.qrc)

set(target qsktestsupport)

if (BUILD_QSKDLL)
    qsk_add_library(${target} SHARED ${SOURCES})
else()
    qsk_add_library(${target} STATIC ${SOURCES})
endif()

target_link_libraries(${target} PUBLIC qskinny)

if (BUILD_QSKDLL)
    set_target_properties( ${target} PROPERTIES DEFINE_SYMBOL SKINNY_MAKEDLL )
endif()

target_compile_definitions(${target}
    PRIVATE PLUGIN_PATH=${CMAKE_BINARY_DIR}/plugins)

target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_LIST_DIR})

if(ENABLE_ENSURE_SKINS)
    target_include_directories(${target} PRIVATE ${QSK_SOURCE_DIR}/designsystems)
    target_compile_definitions(${target} PRIVATE ENSURE_SKINS ENSURE_FONTS)
    target_link_libraries(${target} PRIVATE material3skin fluent2skin fusionskin)
endif()

set_target_properties(${target} PROPERTIES FOLDER libs)
