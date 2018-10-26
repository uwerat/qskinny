QSK_ROOT = $${PWD}/..
QSK_OUT_ROOT = $${OUT_PWD}/..
QSK_PLUGIN_DIR = $${QSK_OUT_ROOT}/plugins

include( $${QSK_ROOT}/qskconfig.pri )
include( $${QSK_ROOT}/qskfunctions.pri )

TEMPLATE = lib
TARGET   = $$qskLibraryTarget(qsktestsupport)

DESTDIR  = $${QSK_OUT_ROOT}/lib

QT += quick 

CONFIG += ensure_skins
CONFIG += fontconfig

contains(QSK_CONFIG, QskDll) {

    CONFIG += dll
    DEFINES += QSK_DLL SKINNY_MAKEDLL
}
else {
    CONFIG += staticlib
}

QSK_DIRS = \
    $${QSK_ROOT}/src/common \
    $${QSK_ROOT}/src/controls \
    $${QSK_ROOT}/src/graphic 

INCLUDEPATH *= $${QSK_DIRS}
DEPENDPATH  *= $${QSK_DIRS}

HEADERS += \
    SkinnyGlobal.h \
    SkinnyFont.h \
    SkinnyShapeFactory.h \
    SkinnyShapeProvider.h \
    SkinnyShortcut.h

SOURCES += \
    SkinnyFont.cpp \
    SkinnyPlugin.cpp \
    SkinnyShapeFactory.cpp \
    SkinnyShapeProvider.cpp \
    SkinnyShortcut.cpp

DEFINES += PLUGIN_PATH=$$clean_path( $$QSK_PLUGIN_DIR )

qskAddLibrary($${QSK_OUT_ROOT}/lib, qskinny)

ensure_skins {

    # Enabling fall back code, that inserts some skin factories manually
    # when not finding skin factories as plugins

    INCLUDEPATH *= $${QSK_ROOT}/skins
    DEPENDPATH *= $${QSK_ROOT}/skins

    DEFINES += ENSURE_SKINS

    QMAKE_RPATHDIR *= $${QSK_PLUGIN_DIR}/skins
    qskAddLibrary($${QSK_PLUGIN_DIR}/skins, squiekskin)
    qskAddLibrary($${QSK_PLUGIN_DIR}/skins, materialskin)
}

fontconfig {

    QSK_FONTDIR=$${PWD}/fonts
    QSK_FONTCACHEDIR=$${OUT_PWD}/fontconfig
    QSK_FONTCONF_FILE=$${QSK_FONTCACHEDIR}/fonts.conf

    QSK_FONTCONF_DATA = "$$cat( $${PWD}/fonts.conf.in, false )"
    QSK_FONTCONF_DATA = $$replace(QSK_FONTCONF_DATA, FONTCACHEDIR, $${QSK_FONTCACHEDIR} )
    QSK_FONTCONF_DATA = $$replace(QSK_FONTCONF_DATA, FONTDIR, $${QSK_FONTDIR} )
    write_file( $${QSK_FONTCONF_FILE}, QSK_FONTCONF_DATA )

    DEFINES += FONTCONFIG_FILE=$$clean_path( $$QSK_FONTCONF_FILE )
}
