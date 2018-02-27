QSK_ROOT = $${PWD}/..
include( $${QSK_ROOT}/qskconfig.pri )

QSK_OUT_ROOT = $${OUT_PWD}/../..

CONFIG         += plugin

QT += quick

TEMPLATE     = lib

QSK_DIRS = \
    $${QSK_ROOT}/src/common \
    $${QSK_ROOT}/src/nodes \
    $${QSK_ROOT}/src/graphic \
    $${QSK_ROOT}/src/controls \
    $${QSK_ROOT}/src/layouts \
    $${QSK_ROOT}/src/dialogs

INCLUDEPATH *= $${QSK_DIRS}
DEPENDPATH  += $${QSK_DIRS}

DESTDIR      = $${QSK_OUT_ROOT}/plugins/skins

QMAKE_RPATHDIR *= $${QSK_OUT_ROOT}/lib
LIBS *= -L$${QSK_OUT_ROOT}/lib -lqskinny

win32 {
    contains(QSK_CONFIG, QskDll) {
        DEFINES    += QT_DLL QSK_DLL
    }
}

