QSK_ROOT = $${PWD}/..
QSK_OUT_ROOT = $${OUT_PWD}/..
QSK_PLUGIN_DIR = $${QSK_OUT_ROOT}/plugins

include( $${QSK_ROOT}/qskconfig.pri )

TEMPLATE = lib
TARGET   = qsktestsupport

DESTDIR  = $${QSK_OUT_ROOT}/lib

QT += quick 
CONFIG += ensure_skins

contains(QSK_CONFIG, QskDll) {

    CONFIG += dll
    DEFINES += QSK_DLL QSK_MAKEDLL
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

RESOURCES += \
    fonts.qrc 

DEFINES += PLUGIN_PATH=$$clean_path( $$QSK_PLUGIN_DIR )

ensure_skins {

    # Enabling fall back code, that inserts some skin factories manually
    # when not finding skin factories as plugins

    INCLUDEPATH *= $${QSK_ROOT}/skins
    DEPENDPATH *= $${QSK_ROOT}/skins

    DEFINES += ENSURE_SKINS

    QMAKE_RPATHDIR *= $${QSK_PLUGIN_DIR}/skins
    LIBS *= -L$${QSK_PLUGIN_DIR}/skins -lsquiekskin -lmaterialskin
}

