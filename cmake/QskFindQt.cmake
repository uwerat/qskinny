############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
# This file may be used under the terms of the QSkinny License, Version 1.0
############################################################################

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
