############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
# This file may be used under the terms of the 3-clause BSD License
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

set(HACK_FONT_CONFIG ON)

if(HACK_FONT_CONFIG)

    message( STATUS "Building an dummy fontconfig enviroment for faster startups" )

    # On systems where many fonts are installed we have a significant effect
    # on the startup performance when building the font cache. To avoid this
    # penalty we configure an empty environment
    # ( this code should be moved to a cmake function TODO ...

    set(QSK_FONTDIR       ${CMAKE_CURRENT_LIST_DIR}/fonts)
    set(QSK_FONTCACHEDIR  ${CMAKE_CURRENT_BINARY_DIR}/fontconfig)
    set(QSK_FONTCONF_FILE ${QSK_FONTCACHEDIR}/fonts.conf)

    configure_file(fonts.conf.cmake.in ${QSK_FONTCONF_FILE}
        @ONLY
        NEWLINE_STYLE LF)

    target_compile_definitions(${target}
        PRIVATE FONTCONFIG_FILE=${QSK_FONTCONF_FILE})
endif()

target_link_libraries(${target} PUBLIC qskinny)

if (BUILD_QSKDLL)
    set_target_properties( ${target} PROPERTIES DEFINE_SYMBOL SKINNY_MAKEDLL )
endif()

target_compile_definitions(${target}
    PRIVATE PLUGIN_PATH=${CMAKE_BINARY_DIR}/plugins)

target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_LIST_DIR})

if(ENABLE_ENSURE_SKINS)
    #set( ENABLE_ENSURE_SKINS OFF )
endif()

if(ENABLE_ENSURE_SKINS)
    target_include_directories(${target} PRIVATE ${CMAKE_SOURCE_DIR}/skins)
    target_compile_definitions(${target} PRIVATE ENSURE_SKINS)
    target_link_libraries(${target} PRIVATE squiekskin material3skin)
endif()

install(TARGETS ${target} DESTINATION ${QSK_INSTALL_LIBS})
