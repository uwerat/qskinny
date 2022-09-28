cmake_minimum_required(VERSION 3.20)

project(qskinny)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(Qt5Core_FIND_COMPONENTS Private CACHE STRING "force include private headers" FORCE)
set(Qt5Gui_FIND_COMPONENTS Private CACHE STRING "force include private headers" FORCE)
set(Qt5OpenGL_FIND_COMPONENTS Private CACHE STRING "force include private headers" FORCE)
set(Qt5Quick_FIND_COMPONENTS Private CACHE STRING "force include private headers" FORCE)
set(Qt5QuickWidgets_FIND_COMPONENTS Private CACHE STRING "force include private headers" FORCE)
set(Qt5Svg_FIND_COMPONENTS Private CACHE STRING "force include private headers" FORCE)
set(Qt5WebEngine_FIND_COMPONENTS Private CACHE STRING "force include private headers" FORCE)
set(Qt5WebEngineCore_FIND_COMPONENTS Private CACHE STRING "force include private headers" FORCE)
set(Qt5Widgets_FIND_COMPONENTS Private CACHE STRING "force include private headers" FORCE)

set(Qt5_DIR "C:/.conan/ea20b9/1/lib/cmake/Qt5") # TODO
find_package(Qt5 REQUIRED COMPONENTS Core Gui Opengl Quick Quickwidgets Svg Webengine WebengineCore Widgets)

message(STATUS "Qt5_VERSION      : ${Qt5_VERSION}")
message(STATUS "Qt5_VERSION_MAJOR: ${Qt5_VERSION_MAJOR}")
message(STATUS "Qt5_VERSION_MINOR: ${Qt5_VERSION_MINOR}")
message(STATUS "Qt5_VERSION_PATCH: ${Qt5_VERSION_PATCH}")

add_subdirectory(src)
add_subdirectory(skins)
#TODO
#add_subdirectory(inputcontext)
add_subdirectory(qmlexport)
add_subdirectory(tools)
add_subdirectory(support)
add_subdirectory(examples)
#add_subdirectory(playground)

#TODO
#OTHER_FILES =
#doc / Doxyfile
#doc/*.dox
#     doc/*.metadox
#     doc/classes/*.dox
#     doc/classes/*.metadox
#     TODO

# TODO
# qmlexport.depends = src
# inputcontext.depends = src
# skins.depends = src
# tools.depends = src
# support.depends = src skins
# examples.depends = tools support skins qmlexport
# playground.depends = tools support skins qmlexport
