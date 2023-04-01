cmake_minimum_required(VERSION 3.16.3)

project(QSkinny
    LANGUAGES C CXX 
    HOMEPAGE_URL "https://github.com/uwerat/qskinny"
    VERSION 999.99.9)

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

option(ENABLE_ENSURE_SKINS "Examples will load skins manually, when not finding plugins" ON)
if ( NOT ( BUILD_EXAMPLES OR BUILD_PLAYGROUND ) )
    set( ENABLE_ENSURE_SKINS OFF )
endif()

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#set(CMAKE_FIND_DEBUG_MODE TRUE)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include("QskFindQt")
include("QskAutogen")
include("QskBuild")

add_subdirectory(src)
add_subdirectory(skins)

if(BUILD_QML_EXPORT)
    add_subdirectory(qmlexport)
endif()

if(BUILD_INPUTCONTEXT)
    add_subdirectory(inputcontext)
endif()

if(BUILD_EXAMPLES OR BUILD_EXAMPLES)
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
