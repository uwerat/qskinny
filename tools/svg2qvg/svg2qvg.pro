QSK_ROOT = $${PWD}/../..
include( $${QSK_ROOT}/qskconfig.pri )

QSK_OUT_ROOT = $${OUT_PWD}/../..

TEMPLATE     = app

QT += svg

QSK_DIRS = \
    $${QSK_ROOT}/src/common \
    $${QSK_ROOT}/src/graphic

INCLUDEPATH *= $${QSK_DIRS}
DEPENDPATH  += $${QSK_DIRS}

DESTDIR      = $${QSK_OUT_ROOT}/tools/bin

QMAKE_RPATHDIR *= $${QSK_OUT_ROOT}/lib
LIBS *= -L$${QSK_OUT_ROOT}/lib -lqskinny

contains(QSK_CONFIG, QskDll) {
    DEFINES    += QSK_DLL
} 

TARGET = svg2qvg

SOURCES += \
    main.cpp
