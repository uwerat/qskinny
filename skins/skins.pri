QSK_ROOT = $${PWD}/..
QSK_OUT_ROOT = $${OUT_PWD}/../..

CONFIG         += plugin

QT += quick

TEMPLATE     = lib
VERSION      = $${QSK_VERSION}

QSK_DIRS = \
    $${QSK_ROOT}/src/common \
    $${QSK_ROOT}/src/nodes \
    $${QSK_ROOT}/src/graphic \
    $${QSK_ROOT}/src/controls \
    $${QSK_ROOT}/src/layouts \
    $${QSK_ROOT}/src/dialogs \
    $${QSK_ROOT}/src/inputpanel

INCLUDEPATH *= $${QSK_DIRS}
DEPENDPATH  += $${QSK_DIRS}

DESTDIR      = $${QSK_OUT_ROOT}/plugins/skins

qskAddLibrary($${QSK_OUT_ROOT}/lib, qskinny)

contains(QSK_CONFIG, QskDll) {
    DEFINES    += QT_DLL QSK_DLL
}

target.path    = $${QSK_INSTALL_PLUGINS}/skins
INSTALLS       = target
