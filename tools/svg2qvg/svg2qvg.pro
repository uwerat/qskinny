QSK_ROOT = $${PWD}/../..
include( $${QSK_ROOT}/qskconfig.pri )

QSK_OUT_ROOT = $${OUT_PWD}/../..

TEMPLATE     = app

QT += svg

CONFIG += standalone

QSK_DIRS = \
    $${QSK_ROOT}/src/common \
    $${QSK_ROOT}/src/graphic

INCLUDEPATH *= $${QSK_DIRS}
DEPENDPATH  += $${QSK_DIRS}

DESTDIR      = $${QSK_OUT_ROOT}/tools/bin

standalone {

    # We only need a very small subset of QSkinny and by including the 
    # necessary cpp files svg2qvg becomes independent from the library

    DEFINES += QSK_STANDALONE

}
else {

    QMAKE_RPATHDIR *= $${QSK_OUT_ROOT}/lib
    LIBS *= -L$${QSK_OUT_ROOT}/lib -lqskinny

    contains(QSK_CONFIG, QskDll) {
        DEFINES    += QSK_DLL
    }
}

TARGET = svg2qvg

SOURCES += \
    main.cpp
