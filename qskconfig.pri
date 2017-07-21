CONFIG           += warn_on
CONFIG           += no_keywords
CONFIG           += silent
CONFIG           -= depend_includepath

CONFIG           += debug
CONFIG           += strict_c++
#CONFIG           += c++14
CONFIG           += c++11
CONFIG           += pedantic

MOC_DIR      = moc
OBJECTS_DIR  = obj
RCC_DIR      = rcc

QSK_CONFIG += QskDll

linux-g++ | linux-g++-64 {

    # CONFIG           += separate_debug_info

    # --- optional warnings

    pedantic {

        # Qt headers do not stand pedantic checks, so it's better
        # to exclude them by declaring them as system includes

        QMAKE_CXXFLAGS += \
            -isystem $$[QT_INSTALL_HEADERS] \
            -isystem $$[QT_INSTALL_HEADERS]/QtCore \
            -isystem $$[QT_INSTALL_HEADERS]/QtGui \
            -isystem $$[QT_INSTALL_HEADERS]/QtQuick \
            -isystem $$[QT_INSTALL_HEADERS]/QtQml

        QMAKE_CXXFLAGS *= -pedantic-errors
        QMAKE_CXXFLAGS *= -Wextra
        QMAKE_CXXFLAGS *= -Werror=format-security
        QMAKE_CXXFLAGS *= -Wlogical-op

        # QMAKE_CXXFLAGS *= -Wconversion
        # QMAKE_CXXFLAGS *= -Wfloat-equal
        # QMAKE_CXXFLAGS *= -Wshadow

        GCC_VERSION = $$system("$$QMAKE_CXX -dumpversion")
        !contains(GCC_VERSION, 4.[0-9]) {
            QMAKE_CXXFLAGS *= -Wsuggest-override
            #QMAKE_CXXFLAGS *= -Wsuggest-final-types
            #QMAKE_CXXFLAGS *= -Wsuggest-final-methods
        }
    }


    # --- optional debug options

    QMAKE_CXXFLAGS_DEBUG   *= -fsanitize=address -fno-omit-frame-pointer
    QMAKE_LFLAGS_DEBUG *= -fsanitize=address

    # --- optional optimzations

    #QMAKE_CXXFLAGS_DEBUG  *= -Og
    QMAKE_CXXFLAGS_DEBUG  *= -O0
    QMAKE_CXXFLAGS_RELEASE  *= -O3

    # QMAKE_CXXFLAGS_RELEASE  *= -Ofast
    # QMAKE_CXXFLAGS_RELEASE  *= -Os
}

debug {
    DEFINES += ITEM_STATISTICS=1
}

# Help out Qt Creator
ide: DEFINES += QT_IDE
