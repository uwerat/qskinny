cmake_minimum_required(VERSION 3.18)

project(iotdashboard_smoketest)

find_package(QSkinny REQUIRED)

find_package(Qt6 COMPONENTS Core QUIET)
if (NOT Qt6_FOUND)
    find_package(Qt5 5.15 REQUIRED COMPONENTS Core Gui OpenGL Quick Svg Widgets)
    find_package(Qt5 5.15 OPTIONAL_COMPONENTS QuickWidgets WebEngine WebEngineCore)
    message(WARNING "using QSkinny's 'qt_add_executable()'")
    function(qt_add_executable)
        add_executable(${ARGV})
    endfunction(qt_add_executable)
    message(WARNING "using QSkinny's 'qt_add_library()'")
    function(qt_add_library)
        add_library(${ARGV})
    endfunction(qt_add_library)
else()
    find_package(Qt6 REQUIRED COMPONENTS Core Gui OpenGL Quick QuickWidgets Svg Widgets)
    find_package(Qt6 OPTIONAL_COMPONENTS QuickWidgets WebEngineCore WebEngineQuick)
endif()

add_subdirectory(../iotdashboard ${CMAKE_CURRENT_BINARY_DIR}/../iotdashboard)