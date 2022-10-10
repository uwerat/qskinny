cmake_minimum_required(VERSION 3.20)

project(qskinny VERSION 999.99.9) # TODO get from git branch name or 

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# force include Qt's private headers
set(Qt5Core_FIND_COMPONENTS          Private CACHE STRING "force include private headers" FORCE)
set(Qt5Gui_FIND_COMPONENTS           Private CACHE STRING "force include private headers" FORCE)
set(Qt5OpenGL_FIND_COMPONENTS        Private CACHE STRING "force include private headers" FORCE)
set(Qt5Quick_FIND_COMPONENTS         Private CACHE STRING "force include private headers" FORCE)
set(Qt5QuickWidgets_FIND_COMPONENTS  Private CACHE STRING "force include private headers" FORCE)
set(Qt5Svg_FIND_COMPONENTS           Private CACHE STRING "force include private headers" FORCE)
set(Qt5WebEngine_FIND_COMPONENTS     Private CACHE STRING "force include private headers" FORCE)
set(Qt5WebEngineCore_FIND_COMPONENTS Private CACHE STRING "force include private headers" FORCE)
set(Qt5Widgets_FIND_COMPONENTS       Private CACHE STRING "force include private headers" FORCE)

set(Qt5_DIR "C:/.conan/ea20b9/1/lib/cmake/Qt5" CACHE PATH "") # TODO pass in externally
find_package(Qt5 REQUIRED COMPONENTS Core Gui Opengl Quick Quickwidgets Svg Webengine WebengineCore Widgets)

# define install directories
set(QSK_INSTALL_DOCS     doc)
set(QSK_INSTALL_HEADERS  include)
set(QSK_INSTALL_LIBS     lib)
set(QSK_INSTALL_BINS     bin)
set(QSK_INSTALL_PLUGINS  plugins)
set(QSK_INSTALL_EXAMPLES examples)

add_subdirectory(src)
add_subdirectory(skins)
add_subdirectory(inputcontext)
add_subdirectory(qmlexport)
add_subdirectory(tools)
add_subdirectory(support)
add_subdirectory(examples)
add_subdirectory(playground)

#TODO add doxygen support
#OTHER_FILES =
#doc / Doxyfile
#doc/*.dox
#     doc/*.metadox
#     doc/classes/*.dox
#     doc/classes/*.metadox
#     TODO