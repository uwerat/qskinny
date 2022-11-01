############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
# This file may be used under the terms of the QSkinny License, Version 1.0
############################################################################

set(QSK_VERSION ${CMAKE_PROJECT_VERSION})

# trying the PREFIX environment variable first
set(QSK_INSTALL_PREFIX $ENV{PREFIX}) # TODO still required?

if("${QSK_INSTALL_PREFIX}" STREQUAL "")
    set(QSK_INSTALL_PREFIX "${CMAKE_INSTALL_PREFIX}") # TODO where does QT_INSTALL_PREFIX come from?

    if (UNIX)
        set(QSK_INSTALL_PREFIX /usr/local/qskinny-${QSK_VERSION})
    endif()

    if (MSVC)
        set(QSK_INSTALL_PREFIX C:/Qskinny-${QSK_VERSION})
    endif()
endif()

# TODO find compiler flag equivalent
list(APPEND CONFIG        no_private_qt_headers_warning)
list(APPEND CONFIG        warn_on)
list(APPEND CONFIG        no_keywords)
list(APPEND CONFIG        silent)
#list(APPEND CONFIG           -= depend_includepath) # TODO was -=
list(APPEND CONFIG        strict_c++)
list(APPEND CONFIG        c++11)
list(APPEND CONFIG        pedantic)
list(APPEND CONFIG        hide_symbols)
#list(APPEND CONFIG           += debug)
#list(APPEND CONFIG           += sanitize)

# TODO
# use_no_rpath {
#     CONFIG -= use_local_rpath use_install_rpath
# } else {
#     cross_compile {
#         CONFIG *= use_install_rpath
#         QMAKE_RPATHLINKDIR *= $${QSK_PLUGIN_DIR}/skins
#     } else {
#         !use_install_rpath: CONFIG *= use_local_rpath
#     }
# }

option(BUILD_QSKDLL             "Build qskinny as shared library" ON)
option(BUILD_EXAMPLES           "Build qskinny examples" ON)
option(BUILD_PLAYGROUND         "Build qskinny playground" ON)
option(BUILD_SVG2QVG_STANDALONE "If 'ON' application won't be linked against qskinny library" ON)
option(ENABLE_PINYIN            "Enable romanization system for standard mandarin chinese" OFF) # TODO try find pkg-config
option(ENABLE_HUNSPELL          "Enable spell checker" OFF) # TODO try find pkg-config
option(ENABLE_FONT_CONFIG       "Enable font config for font caching" ON)
option(ENABLE_ENSURE_SKINS      "Enabling fall back code, that inserts some skin factories manually when not finding skin factories as plugins" ON)
option(ENABLE_AUTOQVG           "enable automatic qvg compilation" ON)

if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

if(LINUX) 

    if ("pedantic" IN_LIST CONFIG)

        add_compile_definitions(QT_STRICT_ITERATORS)

        # Qt headers do not stand pedantic checks, so it's better
        # to exclude them by declaring them as system includes

        list(APPEND CONFIG qtassysteminclude)
    endif()

    if ("qtassysteminclude" IN_LIST CONFIG)

        # As most distros set QT_INSTALL_HEADERS to /usr/include we
        # would run into gcc compiler errors and better drop it
        # from the list below. Should be no problem as we don't
        # add the Qt module to our includes and therefore don't
        # need this path.

        # TODO find solution for all platforms
        # QMAKE_CXXFLAGS += \
        #     -isystem $$[QT_INSTALL_HEADERS]/QtCore \
        #     -isystem $$[QT_INSTALL_HEADERS]/QtCore/$$[QT_VERSION]/QtCore \
        #     -isystem $$[QT_INSTALL_HEADERS]/QtGui \
        #     -isystem $$[QT_INSTALL_HEADERS]/QtGui/$$[QT_VERSION]/QtGui \
        #     -isystem $$[QT_INSTALL_HEADERS]/QtQuick \
        #     -isystem $$[QT_INSTALL_HEADERS]/QtQuick/$$[QT_VERSION]/QtQuick \
        #     -isystem $$[QT_INSTALL_HEADERS]/QtQml \
        #     -isystem $$[QT_INSTALL_HEADERS]/QtQml/$$[QT_VERSION]/QtQml \
    endif()
endif()

if ("${CMAKE_CXX_COMPILER}" MATCHES ".*linux-g\\+\\+" OR "${CMAKE_CXX_COMPILER}" MATCHES ".*linux-g\\+\\+-64")
    # --- optional optimzations
    if (CMAKE_BUILD_TYPE EQUAL "Debug")
        set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -O0) # TODO check if still required
    else()
        set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -O3 -ffast-math) # TODO check if still required
    endif()
endif()

if ("pedantic" IN_LIST CONFIG)
    if ("${CMAKE_CXX_COMPILER}" MATCHES ".*linux-g\\+\\+" OR "${CMAKE_CXX_COMPILER}" MATCHES ".*linux-g\\+\\+-64")
        set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -pedantic-errors)
        set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wextra)
        set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Werror=format-security)
        set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wsuggest-override)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wlogical-op)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wconversion)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wfloat-equal)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wshadow)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wsuggest-final-types)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wsuggest-final-methods)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fanalyzer)
    endif()

    if ("${CMAKE_CXX_COMPILER}" MATCHES ".*linux-clang")

        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -pedantic-errors)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Weverything)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-c++98-compat-pedantic)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-global-constructors)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-exit-time-destructors)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-padded)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-float-equal)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-undefined-reinterpret-cast)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-deprecated)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-switch-enum)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-keyword-macro)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-old-style-cast)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-used-but-marked-unused)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-weak-vtables)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-shadow)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-double-promotion)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-conversion)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-documentation-unknown-command)
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -Wno-unused-macros)
    endif()
endif()

if ("sanitize" IN_LIST CONFIG)

    list(APPEND CONFIG sanitizer)
    list(APPEND CONFIG sanitize_address)
    # CONFIG *= sanitize_undefined

    if ("${CMAKE_CXX_COMPILER}" MATCHES ".*linux-g\\+\\+" OR "${CMAKE_CXX_COMPILER}" MATCHES ".*linux-g\\+\\+-64")
        # set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -fsanitize-address-use-after-scope)
        #QMAKE_LFLAGS *= -fsanitize-address-use-after-scope
    endif()
endif()

if (CMAKE_BUILD_TYPE EQUAL "Debug")
    add_compile_definitions(ITEM_STATISTICS=1)
endif()

# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000

set(LOCAL_PRI $ENV{QSK_LOCAL_PRI})

if (${LOCAL_PRI}) # TODO not empty check

    if(EXISTS  ${LOCAL_PRI})

        # When not working with the Qt/Creator it is often more convenient
        # to include the specific options of your local build, than passing
        # them all on the command line

        include(${LOCAL_PRI})
    endif()
endif()