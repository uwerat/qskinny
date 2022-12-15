cmake_minimum_required(VERSION 3.16.3)

project(QSkinny VERSION 999.99.9)

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include(${CMAKE_SOURCE_DIR}/features/qskconfig.cmake)

# find either Qt5 or Qt6 requred and optional components
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
    find_package(Qt6 OPTIONAL_COMPONENTS QuickWidgets WebEngineCore WebEngineQuick) # check optional QuickWidgets Svg Widgets 
    set(Qt Qt${Qt6_VERSION_MAJOR})
    set(Qt_VERSION       ${Qt6_VERSION})
    set(Qt_VERSION_MAJOR ${Qt6_VERSION_MAJOR})
    set(Qt_VERSION_MINOR ${Qt6_VERSION_MINOR})
    set(Qt_VERSION_PATCH ${Qt6_VERSION_PATCH})
else()
    message(FATAL_ERROR "Neither Qt5_DIR nor Qt6_DIR defined!")
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
    LIBRARY DESTINATION       ${QSK_INSTALL_LIBS}
    ARCHIVE DESTINATION       ${QSK_INSTALL_LIBS}
    RUNTIME DESTINATION       ${QSK_INSTALL_LIBS}
    INCLUDES DESTINATION      ${QSK_INSTALL_HEADERS}
    PUBLIC_HEADER DESTINATION ${QSK_INSTALL_HEADERS})

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