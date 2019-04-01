QSK_ROOT = $${PWD}/..
QSK_OUT_ROOT = $${OUT_PWD}/../..

QT += quick
QT += quick-private
CONFIG += no_private_qt_headers_warning

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
DEPENDPATH  += $${QSK_DIRS}

DESTDIR      = $${QSK_OUT_ROOT}/playground/bin

QMAKE_RPATHDIR *= $${QSK_OUT_ROOT}/lib
qskAddLibrary($${QSK_OUT_ROOT}/lib, qsktestsupport)
qskAddLibrary($${QSK_OUT_ROOT}/lib, qskinny)

contains(QSK_CONFIG, QskDll) {
    DEFINES    += QSK_DLL
}
