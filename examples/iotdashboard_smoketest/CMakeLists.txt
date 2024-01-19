cmake_minimum_required(VERSION 3.18)

project(iotdashboard_smoketest)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC OFF)
set(CMAKE_GLOBAL_AUTOGEN_TARGET OFF)

find_package(QSkinny REQUIRED)

# TODO we don't delivery the qsk macros
function(qsk_add_executable target)
    if(QT_VERSION_MAJOR VERSION_GREATER_EQUAL 6)
        qt6_add_executable(${ARGV})
    else()
        add_executable(${ARGV})
    endif()
endfunction()

# TODO we don't delivery the qsk macros
function(qsk_add_example target)
    cmake_parse_arguments(PARSE_ARGV 1 arg "MANUAL_FINALIZATION" "" "")
    add_executable(${target} WIN32 MACOSX_BUNDLE ${arg_UNPARSED_ARGUMENTS} )
    target_link_libraries(${target} PRIVATE Qsk::QSkinny )
    target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_LIST_DIR})
endfunction()

find_package(Qt6 COMPONENTS Core QUIET)
if (NOT Qt6_FOUND)
    find_package(Qt5 5.15 REQUIRED COMPONENTS Core Gui OpenGL Quick Svg Widgets)
    find_package(Qt5 5.15 OPTIONAL_COMPONENTS QuickWidgets WebEngine WebEngineCore)

    function(qt_add_executable)
        add_executable(${ARGV})
    endfunction(qt_add_executable)
else()
    find_package(Qt6 REQUIRED COMPONENTS Core Gui OpenGL Quick QuickWidgets Svg Widgets)
    find_package(Qt6 OPTIONAL_COMPONENTS QuickWidgets WebEngineCore WebEngineQuick)
endif()

add_subdirectory(../iotdashboard ${CMAKE_CURRENT_BINARY_DIR}/../iotdashboard)

# TODO we don't delivery the support library
get_target_property(iotdashboard_COMPILE_DEFINITIONS iotdashboard COMPILE_DEFINITIONS)
list(FILTER iotdashboard_COMPILE_DEFINITIONS EXCLUDE REGEX [[^USE_SHORTCUTS=1$]])
set_property(TARGET iotdashboard PROPERTY COMPILE_DEFINITIONS ${iotdashboard_COMPILE_DEFINITIONS})

# generate qvg consuming dummy target
qsk_svg2qvg(
    ../iotdashboard/images/ac.svg
    ${CMAKE_CURRENT_BINARY_DIR}/ac.qvg)

file(WRITE ${CMAKE_CURRENT_BINARY_DIR}/main.cpp "int main(){}")
add_executable(test_svg_qgv 
    ${CMAKE_CURRENT_BINARY_DIR}/main.cpp 
    ${CMAKE_CURRENT_BINARY_DIR}/ac.qvg)