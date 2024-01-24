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
    target_compile_definitions(${target} PRIVATE ENSURE_SKINS)
    target_link_libraries(${target} PRIVATE squiekskin material3skin fluent2skin fusionskin)
endif()

set(HIDE_SYSTEM_FONTS ON)

if(HIDE_SYSTEM_FONTS)
    message( STATUS "Hiding system fonts from the examples" )

    # The fonts being used in the examples are included and we do not
    # need the fonts from the system. To avoid, that setting up the font
    # cache slows doesn the stratup performance we hide the system fonts
    # be using a dummy font config environment.
    # ( this hack is pretty old ( Qt 5.4 ) - maybe it is not needed anymore TODO )
    
    set(QSK_FONTDIR       ${CMAKE_CURRENT_LIST_DIR}/fonts)
    set(QSK_FONTCACHEDIR  ${CMAKE_CURRENT_BINARY_DIR}/fontconfig)
    set(QSK_FONTCONF_FILE ${QSK_FONTCACHEDIR}/fonts.conf)
    
    configure_file(fonts.conf.cmake.in ${QSK_FONTCONF_FILE}
        @ONLY NEWLINE_STYLE LF)

    target_compile_definitions(${target}
        PRIVATE FONTCONFIG_FILE=${QSK_FONTCONF_FILE})
endif()

set_target_properties(${target} PROPERTIES FOLDER libs)
