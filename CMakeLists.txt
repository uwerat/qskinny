cmake_minimum_required(VERSION 3.16.3)

project(QSkinny
    LANGUAGES CXX 
    HOMEPAGE_URL "https://github.com/uwerat/qskinny"
    VERSION 999.99.9)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#set(CMAKE_FIND_DEBUG_MODE TRUE)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core)

if ( QT_FOUND )
    message(STATUS "Found Qt ${QT_VERSION} ${_qt_cmake_dir}")

    if(QT_VERSION VERSION_LESS "5.15.0")
        message(FATAL_ERROR "Couldn't find any Qt >= 5.15 !")
    endif()
else()
    message(FATAL_ERROR "Couldn't find any Qt package !")
endif()

# Note, that modules might be found in different places.
# f.e. when having Qt installations that have only been built
# with a subset of modules

find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Gui OpenGL Quick )
find_package(Qt${QT_VERSION_MAJOR} OPTIONAL_COMPONENTS Svg)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include("QskBuild")

# optional package hunspell
if(NOT LINUX AND ENABLE_HUNSPELL)
    message(WARNING "Feature 'hunspell' only available for UNIX!")
    unset(ENABLE_HUNSPELL CACHE)
endif()

# optional package pinyin
if(NOT LINUX AND ENABLE_PINYIN)
    message(WARNING "Feature 'pinyin' only available for UNIX!")
    unset(ENABLE_PINYIN CACHE)
endif()

if(ENABLE_HUNSPELL)
    include("QSkinnyOptionalHunspell")
endif()

if(ENABLE_PINYIN)
    include("QSkinnyOptionalPinyin")
endif()

add_subdirectory(src)
add_subdirectory(skins)
add_subdirectory(inputcontext)
add_subdirectory(qmlexport)
add_subdirectory(tools)
add_subdirectory(support)

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()
if(BUILD_PLAYGROUND)
    add_subdirectory(playground)
endif()
