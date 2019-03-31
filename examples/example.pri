QSK_ROOT = $${PWD}/..
QSK_OUT_ROOT = $${OUT_PWD}/../..

QT += quick

TEMPLATE     = app

QSK_DIRS = \
    $${QSK_ROOT}/src/common \
    $${QSK_ROOT}/src/nodes \
    $${QSK_ROOT}/src/graphic \
    $${QSK_ROOT}/src/controls \
    $${QSK_ROOT}/src/layouts \
    $${QSK_ROOT}/src/dialogs \
    $${QSK_ROOT}/src/inputpanel \
    $${QSK_ROOT}/support

INCLUDEPATH *= $${QSK_DIRS}
DEPENDPATH  *= $${QSK_DIRS}

DESTDIR      = $${QSK_OUT_ROOT}/examples/bin

QMAKE_RPATHDIR *= $${QSK_OUT_ROOT}/lib
qskAddLibrary($${QSK_OUT_ROOT}/lib, qsktestsupport)
qskAddLibrary($${QSK_OUT_ROOT}/lib, qskinny)

win32 {
    contains(QSK_CONFIG, QskDll) {
        DEFINES    += QT_DLL QSK_DLL
    }
}

