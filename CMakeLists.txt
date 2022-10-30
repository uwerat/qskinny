cmake_minimum_required(VERSION 3.16.3)

project(qskinny VERSION 999.99.9) # TODO get from git branch name or 

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

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

if("${Qt5_DIR}" STREQUAL "" AND "${Qt6_DIR}" STREQUAL "")
    message(FATAL_ERROR "Neither Qt5_DIR nor Qt6_DIR defined!")
else()
    # TODO ckeck find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
    if(NOT "${Qt5_DIR}" STREQUAL "")
        find_package(Qt5 REQUIRED COMPONENTS Core Gui OpenGL Quick QuickWidgets Svg Webengine WebengineCore Widgets)
        set(Qt Qt${Qt5_VERSION_MAJOR}) # TODO find better solution for Qt namespace
        set(Qt_VERSION       ${Qt5_VERSION})
        set(Qt_VERSION_MAJOR ${Qt5_VERSION_MAJOR})
        set(Qt_VERSION_MINOR ${Qt5_VERSION_MINOR})
        set(Qt_VERSION_PATCH ${Qt5_VERSION_PATCH})
    elseif(NOT "${Qt6_DIR}" STREQUAL "")
        find_package(Qt6 REQUIRED COMPONENTS Core Gui OpenGL Quick QuickWidgets Svg WebEngineCore WebEngineQuick Widgets)
        set(Qt Qt${Qt6_VERSION_MAJOR}) # TODO find better solution for Qt namespace
        set(Qt_VERSION       ${Qt6_VERSION})
        set(Qt_VERSION_MAJOR ${Qt6_VERSION_MAJOR})
        set(Qt_VERSION_MINOR ${Qt6_VERSION_MINOR})
        set(Qt_VERSION_PATCH ${Qt6_VERSION_PATCH})
    endif()
endif()

if(TRUE)
    message(STATUS "\${Qt}              : ${Qt}")
    message(STATUS "\${Qt_VERSION_MAJOR}: ${Qt_VERSION_MAJOR}")
    message(STATUS "\${Qt_VERSION_MINOR}: ${Qt_VERSION_MINOR}")
    message(STATUS "\${Qt_VERSION_PATCH}: ${Qt_VERSION_PATCH}")
endif()

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