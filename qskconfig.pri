CONFIG           += warn_on
CONFIG           += no_keywords
CONFIG           += silent
CONFIG           -= depend_includepath

CONFIG           += debug
CONFIG           += strict_c++
CONFIG           += c++11
CONFIG           += pedantic

debug: CONFIG           += sanitize

MOC_DIR      = moc
OBJECTS_DIR  = obj
RCC_DIR      = rcc

QSK_CONFIG += QskDll

linux {

    pedantic {

        DEFINES += QT_STRICT_ITERATORS

        # Qt headers do not stand pedantic checks, so it's better
        # to exclude them by declaring them as system includes

        QMAKE_CXXFLAGS += \
            -isystem $$[QT_INSTALL_HEADERS] \
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

    pedantic {

        # --- optional warnings

        QMAKE_CXXFLAGS *= -pedantic-errors
        QMAKE_CXXFLAGS *= -Wextra
        QMAKE_CXXFLAGS *= -Werror=format-security
        QMAKE_CXXFLAGS *= -Wlogical-op

        # QMAKE_CXXFLAGS *= -Wconversion
        # QMAKE_CXXFLAGS *= -Wfloat-equal
        # QMAKE_CXXFLAGS *= -Wshadow

        GCC_VERSION = $$system("$$QMAKE_CXX -dumpversion")

        equals(GCC_VERSION,4) || contains(GCC_VERSION, 4.* ) {

            # gcc 4.x is too old for certain warning options
        }
        else {
            QMAKE_CXXFLAGS *= -Wsuggest-override
            QMAKE_CXXFLAGS *= -Wsuggest-final-types
            QMAKE_CXXFLAGS *= -Wsuggest-final-methods
        }
    }

    # --- optional optimzations

    QMAKE_CXXFLAGS_DEBUG  *= -O0
    #QMAKE_CXXFLAGS_DEBUG  *= -Og

    QMAKE_CXXFLAGS_RELEASE  *= -O3 
    QMAKE_CXXFLAGS_RELEASE  *= -ffast-math

    # QMAKE_CXXFLAGS_RELEASE  *= -Ofast
    # QMAKE_CXXFLAGS_RELEASE  *= -Os
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
