############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
# This file may be used under the terms of the QSkinny License, Version 1.0
############################################################################

QSK_VER_MAJ      = 0
QSK_VER_MIN      = 0
QSK_VER_PAT      = 0
QSK_VERSION      = $${QSK_VER_MAJ}.$${QSK_VER_MIN}.$${QSK_VER_PAT}

QSK_INSTALL_PREFIX    = $$[QT_INSTALL_PREFIX]

unix {
    QSK_INSTALL_PREFIX    = /usr/local/qskinny-$${QSK_VERSION}
}

win32 {
    QSK_INSTALL_PREFIX    = C:/Qskinny-$${QSK_VERSION}
}

QSK_INSTALL_DOCS      = $${QSK_INSTALL_PREFIX}/doc
QSK_INSTALL_HEADERS   = $${QSK_INSTALL_PREFIX}/include
QSK_INSTALL_LIBS      = $${QSK_INSTALL_PREFIX}/lib
QSK_INSTALL_BINS      = $${QSK_INSTALL_PREFIX}/bin
QSK_INSTALL_PLUGINS   = $${QSK_INSTALL_PREFIX}/plugins
QSK_INSTALL_EXAMPLES  = $${QSK_INSTALL_PREFIX}/examples

CONFIG           += no_private_qt_headers_warning

CONFIG           += warn_on
CONFIG           += no_keywords
CONFIG           += silent
CONFIG           -= depend_includepath

CONFIG           += strict_c++
CONFIG           += c++11
CONFIG           += pedantic
CONFIG           += hide_symbols

# CONFIG           += debug
# CONFIG           += sanitize

use_no_rpath {
    CONFIG -= use_local_rpath use_install_rpath
} else {
    cross_compile {
        CONFIG *= use_install_rpath
        QMAKE_RPATHLINKDIR *= $${QSK_PLUGIN_DIR}/skins
    } else {
        !use_install_rpath: CONFIG *= use_local_rpath
    }
}

MOC_DIR      = moc
OBJECTS_DIR  = obj
RCC_DIR      = rcc

QSK_CONFIG += QskDll

*-g++* {

    GCC_VERSION = $$system("$$QMAKE_CXX -dumpversion")
    equals(GCC_VERSION,4) || contains(GCC_VERSION, 4.* ) {

        # gcc4 is too old for certain checks
        CONFIG -= pedantic sanitize
    }
}

linux {

    pedantic {

        DEFINES += QT_STRICT_ITERATORS

        # Qt headers do not stand pedantic checks, so it's better
        # to exclude them by declaring them as system includes

        # As most distros set QT_INSTALL_HEADERS to /usr/include we
        # would run into gcc compiler errors and better drop it
        # from the list below. Should be no problem as we don't
        # add the Qt module to our includes and therefore don't
        # need this path.

        # QMAKE_CXXFLAGS += -isystem $$[QT_INSTALL_HEADERS]

        QMAKE_CXXFLAGS += \
            -isystem $$[QT_INSTALL_HEADERS]/QtCore \
            -isystem $$[QT_INSTALL_HEADERS]/QtGui \
            -isystem $$[QT_INSTALL_HEADERS]/QtGui/$$[QT_VERSION]/QtGui \
            -isystem $$[QT_INSTALL_HEADERS]/QtQuick \
            -isystem $$[QT_INSTALL_HEADERS]/QtQuick/$$[QT_VERSION]/QtQuick \
            -isystem $$[QT_INSTALL_HEADERS]/QtQml \
            -isystem $$[QT_INSTALL_HEADERS]/QtQml/$$[QT_VERSION]/QtQml \
    }
}

linux-g++ | linux-g++-64 {

    # --- optional optimzations

    QMAKE_CXXFLAGS_DEBUG  *= -O0
    #QMAKE_CXXFLAGS_DEBUG  *= -Og

    QMAKE_CXXFLAGS_RELEASE  *= -O3 
    QMAKE_CXXFLAGS_RELEASE  *= -ffast-math

    # QMAKE_CXXFLAGS_RELEASE  *= -Ofast
    # QMAKE_CXXFLAGS_RELEASE  *= -Os

    # Some versions ( here 2.31.1 ) of the BFD linker may generate shared
    # libraries with corrupt symbol version info which leads to
    # "invalid version 21" errors when the corrupt shared library is used. 
    # One possible workaround is to use the gold linker instead.

    # QMAKE_LFLAGS  *= -Wl,-fuse-ld=gold
}

pedantic {

    linux-g++ | linux-g++-64 {

        QMAKE_CXXFLAGS *= -pedantic-errors
        QMAKE_CXXFLAGS *= -Wextra
        QMAKE_CXXFLAGS *= -Werror=format-security
        QMAKE_CXXFLAGS *= -Wlogical-op

        # QMAKE_CXXFLAGS *= -Wconversion
        # QMAKE_CXXFLAGS *= -Wfloat-equal
        # QMAKE_CXXFLAGS *= -Wshadow

        QMAKE_CXXFLAGS *= -Wsuggest-override
        # QMAKE_CXXFLAGS *= -Wsuggest-final-types
        # QMAKE_CXXFLAGS *= -Wsuggest-final-methods

        # QMAKE_CXXFLAGS *= -fanalyzer
    }

    linux-clang {

        #QMAKE_CXXFLAGS *= -pedantic-errors

        #QMAKE_CXXFLAGS *= -Weverything
        #QMAKE_CXXFLAGS *= -Wno-c++98-compat-pedantic
        #QMAKE_CXXFLAGS *= -Wno-global-constructors
        #QMAKE_CXXFLAGS *= -Wno-exit-time-destructors
        #QMAKE_CXXFLAGS *= -Wno-padded
        #QMAKE_CXXFLAGS *= -Wno-float-equal
        #QMAKE_CXXFLAGS *= -Wno-undefined-reinterpret-cast
        #QMAKE_CXXFLAGS *= -Wno-deprecated
        #QMAKE_CXXFLAGS *= -Wno-switch-enum
        #QMAKE_CXXFLAGS *= -Wno-keyword-macro
        #QMAKE_CXXFLAGS *= -Wno-old-style-cast
        #QMAKE_CXXFLAGS *= -Wno-used-but-marked-unused
        #QMAKE_CXXFLAGS *= -Wno-weak-vtables
        #QMAKE_CXXFLAGS *= -Wno-shadow
        #QMAKE_CXXFLAGS *= -Wno-double-promotion
        #QMAKE_CXXFLAGS *= -Wno-conversion
        #QMAKE_CXXFLAGS *= -Wno-documentation-unknown-command
        #QMAKE_CXXFLAGS *= -Wno-unused-macros
    }
}

sanitize {

    CONFIG += sanitizer 
    CONFIG += sanitize_address 
    #CONFIG *= sanitize_memory 
    CONFIG *= sanitize_undefined

    linux-g++ | linux-g++-64 {
        #QMAKE_CXXFLAGS *= -fsanitize-address-use-after-scope
        #QMAKE_LFLAGS *= -fsanitize-address-use-after-scope
    }
}

debug {
    DEFINES += ITEM_STATISTICS=1
}

# Help out Qt Creator
ide: DEFINES += QT_IDE

# DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x000000
