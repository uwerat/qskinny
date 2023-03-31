############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
# This file may be used under the terms of the QSkinny License, Version 1.0
############################################################################

# Often users have several Qt installations on their system and
# need to be able to explicitly the one to be used. Let's see if
# standard cmake features are good enough or if we need to introduce
# something sort of additional option. TODO ...

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)

if ( QT_FOUND )

    # Would like to have a status message about where the Qt installation
    # has been found without having the mess of CMAKE_FIND_DEBUG_MODE
    # All I found was 

    message(STATUS "Found Qt ${QT_VERSION} ${_qt_cmake_dir}")

    if(QT_VERSION VERSION_LESS "5.15.0")
        message(FATAL_ERROR "Couldn't find any Qt >= 5.15 !")
    endif()
else()
    message(FATAL_ERROR "Couldn't find any Qt package !")
endif()

# Not sure if using find_package here is a good idea.
# In situations, where the Qt installation that has been found
# above has not been built with all modules we might end up with modules
# from other Qt installations/versions. Might be better to limit
# finding packages from the same package. TODO ...

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui OpenGL Quick )

if( BUILD_TOOLS )
    # needed for building the svg2qvg tool
    find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Svg)
endif()

# some examples need additional modules

if( BUILD_EXAMPLES OR BUILD_PLAYGROUND )
    find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS QuickWidgets)
endif()

if(BUILD_PLAYGROUND)
    if (QT_VERSION_MAJOR VERSION_LESS 6)
        find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS WebEngine)
    else()
        find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS WebEngineCore WebEngineQuick)
    endif()
endif()
