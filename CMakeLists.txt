############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

cmake_minimum_required(VERSION 3.16.3)

macro(qsk_setup_options)

    option(BUILD_PEDANTIC "Enable pedantic compile flags ( only GNU/CLANG )" OFF)
    option(BUILD_QSKDLL "Build libs ( not plugins ) as shared library" ON)
    option(BUILD_QML_EXPORT "Exporting QSkinny to QML" ON)

    option(BUILD_TOOLS      "Build qskinny tools" ON)
    option(BUILD_INPUTCONTEXT "Build virtual keyboard support" ON)
    option(BUILD_EXAMPLES   "Build qskinny examples" ON)
    option(BUILD_PLAYGROUND "Build qskinny playground" ON)

    # we actually want to use cmake_dependent_option - minimum cmake version ??

    option( BUILD_SVG2QVG_STANDALONE "Build svg2qvg without qskinny library dependency" ON )
    if( NOT BUILD_TOOLS )
        set( BUILD_SVG2QVG_STANDALONE OFF )
    endif()

    if( NOT BUILD_INPUTCONTEXT )
        set( ENABLE_PINYIN OFF )
        set( ENABLE_HUNSPELL OFF )
    endif()

    option(ENABLE_ENSURE_SKINS "Examples add skins manually, when not finding plugins" ON)

endmacro()

macro(qsk_setup_build)

    set(CMAKE_AUTOMOC ON)
    set(CMAKE_AUTORCC ON)
    set(CMAKE_AUTOUIC OFF)
    set(CMAKE_GLOBAL_AUTOGEN_TARGET OFF)
    set(AUTOGEN_BUILD_DIR "${CMAKE_CURRENT_BINARY_DIR}/autogen")

    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    qsk_initialize_build_flags()

    if ( BUILD_PEDANTIC )
        message( STATUS "Setting pedantic compiler flags" )
        qsk_enable_pedantic_flags()
    endif()

    # Loading individual settings from a file that is
    # propagated by the environmant variable QSK_LOCAL_CMAKE_RULES
    # This is a leftover from the previous qmake build environment.
    # Let's if we can do this using cmake standard mechanisms TODO ...

    qsk_load_optional_build_flags()

    # Finalizing after processing QSK_LOCAL_CMAKE_RULES
    qsk_finalize_build_flags()

endmacro()

macro(qsk_setup_install)

    # package vars
    set(PACKAGE_NAME      ${PROJECT_NAME})
    set(PACKAGE_VERSION   ${CMAKE_PROJECT_VERSION})
    set(PACKAGE_NAMESPACE ${PROJECT_NAME}::)
    set(PACKAGE_LOCATION  ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})

    # install paths for headers and libraries
    string(TOLOWER "${PROJECT_NAME}" PACKAGE_DIR)
    set(QSK_INSTALL_HEADERS ${CMAKE_INSTALL_INCLUDEDIR}/${PACKAGE_DIR})
    set(QSK_INSTALL_LIBS    ${CMAKE_INSTALL_LIBDIR}/${PACKAGE_DIR})

endmacro()

############################################################################
# QSkinny
############################################################################

project(QSkinny
    LANGUAGES C CXX 
    HOMEPAGE_URL "https://github.com/uwerat/qskinny"
    VERSION 0.8.0)

set(QSK_SOURCE_DIR ${CMAKE_CURRENT_LIST_DIR})
set(QSK_CMAKE_DIR  ${QSK_SOURCE_DIR}/cmake)

qsk_setup_options()

include(GNUInstallDirs)

# loading helper macros
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

include("QskConfigMacros")
include("QskFindMacros")
include("QskBuildFunctions")

qsk_setup_Qt()
qsk_setup_build()
qsk_setup_install()

add_subdirectory(src)
add_subdirectory(skins)

include(CMakePackageConfigHelpers)

# write QSkinnyConfigVersion.cmake file for cmake import
write_basic_package_version_file(
    ${CMAKE_BINARY_DIR}/_QSkinny/${PACKAGE_NAME}ConfigVersion.cmake
    VERSION ${PACKAGE_VERSION}
    COMPATIBILITY AnyNewerVersion)

# Copy QSkinnyConfig.cmake to build dir
configure_file(${QSK_CMAKE_DIR}/${PACKAGE_NAME}Config.cmake
    ${CMAKE_BINARY_DIR}/_QSkinny/${PACKAGE_NAME}Config.cmake
    COPYONLY)

# install QSkinnyConfig.cmake and QSkinnyConfigVersion.cmake
# and QskTools.cmake file to lib/cmake/QSkinny directory
install(
    FILES
        ${CMAKE_BINARY_DIR}/_QSkinny/${PACKAGE_NAME}Config.cmake
        ${CMAKE_BINARY_DIR}/_QSkinny/${PACKAGE_NAME}ConfigVersion.cmake
    DESTINATION
        ${PACKAGE_LOCATION}
    COMPONENT
        Devel)

# Build other libraries
if(BUILD_QML_EXPORT)
    add_subdirectory(qmlexport)
endif()

if(BUILD_INPUTCONTEXT)
    add_subdirectory(inputcontext)
endif()

if(BUILD_EXAMPLES OR BUILD_PLAYGROUND)
    add_subdirectory(support)
endif()

if(BUILD_TOOLS)
    add_subdirectory(tools)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(BUILD_PLAYGROUND)
    add_subdirectory(playground)
endif()
