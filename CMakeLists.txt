cmake_minimum_required(VERSION 3.16.3)

project(QSkinny
    LANGUAGES C CXX 
    HOMEPAGE_URL "https://github.com/uwerat/qskinny"
    VERSION 999.99.9)

option(BUILD_QSKDLL
    "Build qskinny as shared library" ON)

option(BUILD_TOOLS      "Build qskinny tools" ON)
option(BUILD_EXAMPLES   "Build qskinny examples" ON)
option(BUILD_PLAYGROUND "Build qskinny playground" ON)
option(BUILD_SVG2QVG_STANDALONE "Build svg2qvg without qskinny library dependency" ON)

option(ENABLE_PINYIN    "UNIX: Enable romanization system for standard mandarin chinese" OFF)
option(ENABLE_HUNSPELL  "UNIX: Enable spell checker" OFF)
option(ENABLE_ENSURE_SKINS "Examples will load skins manually, when not finding plugins" ON)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

#set(CMAKE_FIND_DEBUG_MODE TRUE)
set_property(GLOBAL PROPERTY USE_FOLDERS ON)

include("QskFindQt")
include("QskAutogen")
include("QskBuild")

add_subdirectory(src)
add_subdirectory(skins)
add_subdirectory(inputcontext)
add_subdirectory(qmlexport)

if(BUILD_EXAMPLES OR BUILD_EXAMPLES)
    add_subdirectory(support)
endif()

if(BUILD_TOLLS)
    add_subdirectory(tools)
endif()

if(BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(BUILD_PLAYGROUND)
    add_subdirectory(playground)
endif()
