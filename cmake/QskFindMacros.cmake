############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
# This file may be used under the terms of the QSkinny License, Version 1.0
############################################################################

macro(qsk_setup_Qt)

    # Often users have several Qt installations on their system and
    # need to be able to explicitly the one to be used. Let's see if
    # standard cmake features are good enough or if we need to introduce
    # something sort of additional option. TODO ...

    find_package(QT "5.15" NAMES Qt6 Qt5 REQUIRED COMPONENTS Quick)

    if ( QT_FOUND )

        # Would like to have a status message about where the Qt installation
        # has been found without having the mess of CMAKE_FIND_DEBUG_MODE
        # With Qt6 there seems to be: _qt_cmake_dir

        message(STATUS "Found Qt ${QT_VERSION} ${_qt_cmake_dir}")
    else()
        message(FATAL_ERROR "Couldn't find any Qt package !")
    endif()

    # Using find_package is probably no good idea as all other Qt modules should
    # be located below ${_qt_cmake_dir}. Otherwise we might end up with modules
    # from different packages. TODO ...

    if( BUILD_TOOLS )
        # needed for building the svg2qvg tool
        find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Svg)
    endif()

    if( BUILD_EXAMPLES OR BUILD_PLAYGROUND )
        # On embedded systems you often find optimized Qt installations without
        # Qt/Widgets support. QSkinny itself does not need Qt/Widgets - however
        # playground/grids demonstrates the differences between the various
        # grid layouts. So no big deal when it is missing.

        find_package(Qt${QT_VERSION_MAJOR} QUIET OPTIONAL_COMPONENTS QuickWidgets)
        if (NOT Qt${QT_VERSION_MAJOR}QuickWidgets_FOUND)
            message(STATUS "No Qt/Quick Widgets support: skipping some unimportant examples")
        endif()
    endif()

    if(BUILD_PLAYGROUND)
        # When building Qt most of the time is spent on the WebEngine modules
        # while only few applications need it. So it is a common situation to
        # have Qt installations without.
        # playground/webview shows how to use the Qt/Quick frontend from
        # C++, but QSkinny itself does not need the WebEngine at all.

        if (QT_VERSION_MAJOR VERSION_LESS 6)
            find_package(Qt${QT_VERSION_MAJOR} QUIET OPTIONAL_COMPONENTS WebEngine)
            set( Qt5WebEngineQuick_FOUND ${Qt5WebEngine_FOUND} )
        else()
            find_package(Qt${QT_VERSION_MAJOR} QUIET
                OPTIONAL_COMPONENTS WebEngineCore WebEngineQuick)
        endif()

        if( NOT Qt${QT_VERSION_MAJOR}WebEngineQuick_FOUND)
            message(STATUS "No Qt/Quick WebEngine support: skipping some unimportant examples")
        endif()
    endif()
endmacro()

macro(qsk_setup_Hunspell)

    find_package(PkgConfig)

    pkg_check_modules(hunspell hunspell IMPORTED_TARGET)
    set(package_name "PkgConfig::hunspell")

    if (NOT TARGET "${package_name}")
        set(Hunspell_FOUND 0)
        return()
    endif()

    add_library(Hunspell::Hunspell INTERFACE IMPORTED)
    target_link_libraries(Hunspell::Hunspell INTERFACE ${package_name})
    unset(package_name)

    set(Hunspell_FOUND TRUE)

endmacro()

macro(qsk_setup_Pinyin)

    # does this code work ????

    find_package(PkgConfig)
    pkg_check_modules(PKG_PINYIN QUIET pinyin)

    find_path(PINYIN_INCLUDE_DIRS
        NAMES pinyinime.h
        PATH_SUFFIXES pinyin LibIME LibIME/libime/pinyin
        HINTS ${PKG_PINYIN_INCLUDE_DIRS})

    find_library(PINYIN_LIBRARIES
        NAMES ${PKG_PINYIN_LIBRARIES} IMEPinyin imepinyin ime libime libimepinyin
        HINTS ${PKG_PINYIN_LIBRARY_DIRS})

    include(FindPackageHandleStandardArgs)
    find_package_handle_standard_args(PINYIN
        REQUIRED_VARS PINYIN_LIBRARIES PINYIN_INCLUDE_DIRS
        VERSION_VAR PKG_PINYIN_VERSION)

    mark_as_advanced(PINYIN_INCLUDE_DIRS PINYIN_LIBRARIES)

    add_library(pinyin SHARED IMPORTED GLOBAL)
    set_target_properties(pinyin PROPERTIES IMPORTED_LOCATION ${PINYIN_LIBRARIES})

    target_include_directories(pinyin INTERFACE ${PINYIN_INCLUDE_DIRS})

    find_package(LibIMEPinyin REQUIRED)
    find_package(Fcitx5Utils REQUIRED)

endmacro()
