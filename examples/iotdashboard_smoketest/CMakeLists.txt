cmake_minimum_required(VERSION 3.18)

project(iotdashboard_smoketest)

find_package(QSkinny REQUIRED)

if("${Qt5_DIR}" STREQUAL "" AND "${Qt6_DIR}" STREQUAL "")
    message(FATAL_ERROR "Neither Qt5_DIR nor Qt6_DIR defined!")
else()
    # force include Qt's private headers
    if(NOT "${Qt5_DIR}" STREQUAL "")
        set(Qt5Core_FIND_COMPONENTS          Private CACHE STRING "force include private headers" FORCE)
        set(Qt5Gui_FIND_COMPONENTS           Private CACHE STRING "force include private headers" FORCE)
        set(Qt5OpenGL_FIND_COMPONENTS        Private CACHE STRING "force include private headers" FORCE)
        set(Qt5Quick_FIND_COMPONENTS         Private CACHE STRING "force include private headers" FORCE)
        set(Qt5QuickWidgets_FIND_COMPONENTS  Private CACHE STRING "force include private headers" FORCE)
        set(Qt5Svg_FIND_COMPONENTS           Private CACHE STRING "force include private headers" FORCE)
        set(Qt5WebEngine_FIND_COMPONENTS     Private CACHE STRING "force include private headers" FORCE)
        set(Qt5WebEngineCore_FIND_COMPONENTS Private CACHE STRING "force include private headers" FORCE)
        set(Qt5Widgets_FIND_COMPONENTS       Private CACHE STRING "force include private headers" FORCE)
    elseif(NOT "${Qt6_DIR}" STREQUAL "")
        #set(Qt6Core_FIND_COMPONENTS          Private CACHE STRING "force include private headers" FORCE)
        #set(Qt6Gui_FIND_COMPONENTS           Private CACHE STRING "force include private headers" FORCE)
        set(Qt6OpenGL_FIND_COMPONENTS        Private CACHE STRING "force include private headers" FORCE)
        #set(Qt6Quick_FIND_COMPONENTS         Private CACHE STRING "force include private headers" FORCE)
        #set(Qt6QuickWidgets_FIND_COMPONENTS  Private CACHE STRING "force include private headers" FORCE)
        #set(Qt6Svg_FIND_COMPONENTS           Private CACHE STRING "force include private headers" FORCE)
        set(Qt6WebEngine_FIND_COMPONENTS     Private CACHE STRING "force include private headers" FORCE)
        set(Qt6WebEngineCore_FIND_COMPONENTS Private CACHE STRING "force include private headers" FORCE)
        set(Qt6WebEngineQuick_FIND_COMPONENTS Private CACHE STRING "force include private headers" FORCE)
        #set(Qt6Widgets_FIND_COMPONENTS       Private CACHE STRING "force include private headers" FORCE)
    endif()
endif()

if(NOT "${Qt5_DIR}" STREQUAL "")
    find_package(Qt5 REQUIRED COMPONENTS Core Gui OpenGL Quick Svg Widgets)
    find_package(Qt5 OPTIONAL_COMPONENTS QuickWidgets WebEngine WebEngineCore)
    set(Qt Qt${Qt5_VERSION_MAJOR})
    set(Qt_VERSION       ${Qt5_VERSION})
    set(Qt_VERSION_MAJOR ${Qt5_VERSION_MAJOR})
    set(Qt_VERSION_MINOR ${Qt5_VERSION_MINOR})
    set(Qt_VERSION_PATCH ${Qt5_VERSION_PATCH})
elseif(NOT "${Qt6_DIR}" STREQUAL "")

    string(REPLACE "/lib/cmake/Qt6" "" Qt6_PREFIX_DIR "${Qt6_DIR}")
    set(CMAKE_PREFIX_PATH ${CMAKE_PREFIX_PATH} ${Qt6_PREFIX_DIR})

    find_package(Qt6 REQUIRED COMPONENTS Core Gui OpenGL Quick QuickWidgets Svg Widgets)
    find_package(Qt6 OPTIONAL_COMPONENTS QuickWidgets WebEngineCore WebEngineQuick)
    set(Qt Qt${Qt6_VERSION_MAJOR})
    set(Qt_VERSION       ${Qt6_VERSION})
    set(Qt_VERSION_MAJOR ${Qt6_VERSION_MAJOR})
    set(Qt_VERSION_MINOR ${Qt6_VERSION_MINOR})
    set(Qt_VERSION_PATCH ${Qt6_VERSION_PATCH})
endif()

add_subdirectory(../iotdashboard ${CMAKE_CURRENT_BINARY_DIR}/../iotdashboard)