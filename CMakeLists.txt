cmake_minimum_required(VERSION 3.16.3)

project(QSkinny VERSION 999.99.9) # TODO get from git branch name or 

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(${CMAKE_SOURCE_DIR}/features/qskconfig.cmake)

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
    include(cmake/QSkinnyOptionalHunspell.cmake)
endif()

if(ENABLE_PINYIN)
    include(cmake/QSkinnyOptionalPinyin.cmake)
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

# define debug postfix globally
set(CMAKE_DEBUG_POSTFIX d)
# define binary install directory globally
set(CMAKE_INSTALL_BINDIR ${QSK_INSTALL_BINS}$<$<CONFIG:Debug>:/debug>)
# define library install directory globally
# set(CMAKE_INSTALL_LIBDIR lib$<$<CONFIG:Debug>:/debug>)
# define install rpath globally
# set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/${QSK_INSTALL_LIBS})

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

#TODO add doxygen support
#OTHER_FILES =
#doc / Doxyfile
#doc/*.dox
#     doc/*.metadox
#     doc/classes/*.dox
#     doc/classes/*.metadox

set(PACKAGE_NAME      ${PROJECT_NAME})
set(PACKAGE_VERSION   ${CMAKE_PROJECT_VERSION})
set(PACKAGE_NAMESPACE Qsk)
set(PACKAGE_LOCATION  lib/cmake/${PROJECT_NAME})

install(TARGETS qskinny EXPORT ${PACKAGE_NAME}Targets
    LIBRARY DESTINATION  ${QSK_INSTALL_LIBS}
    ARCHIVE DESTINATION  ${QSK_INSTALL_LIBS}
    RUNTIME DESTINATION  ${QSK_INSTALL_LIBS}
    INCLUDES DESTINATION ${QSK_INSTALL_HEADERS})

include(CMakePackageConfigHelpers)
write_basic_package_version_file(
    ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/${PACKAGE_NAME}ConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion)

export(EXPORT ${PACKAGE_NAME}Targets
    FILE ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/${PACKAGE_NAME}Targets.cmake
    NAMESPACE ${PACKAGE_NAMESPACE}::)

configure_file(cmake/${PACKAGE_NAME}Config.cmake
    ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/${PACKAGE_NAME}Config.cmake
    COPYONLY)

install(EXPORT ${PACKAGE_NAME}Targets
    FILE
        ${PACKAGE_NAME}Targets.cmake
    NAMESPACE
        ${PACKAGE_NAMESPACE}::
    DESTINATION
        ${PACKAGE_LOCATION})

install(
    FILES
        cmake/${PACKAGE_NAME}Config.cmake
        cmake/${PACKAGE_NAME}Macros.cmake
        cmake/${PACKAGE_NAME}OptionalHunspell.cmake
        cmake/${PACKAGE_NAME}OptionalPinyin.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/${PACKAGE_NAME}/${PACKAGE_NAME}ConfigVersion.cmake
    DESTINATION
        ${PACKAGE_LOCATION}
    COMPONENT
        Devel)