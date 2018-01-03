QSK_ROOT = $${PWD}/..
QSK_OUT_ROOT = $${OUT_PWD}/..
QSK_PLUGIN_DIR = $${QSK_OUT_ROOT}/plugins

include( $${QSK_ROOT}/qskconfig.pri )

TEMPLATE = lib
TARGET   = qsktestsupport

DESTDIR  = $${QSK_OUT_ROOT}/lib

QT += quick 

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

QSK_DIRS += $${QSK_ROOT}/skins

DEFINES += PLUGIN_PATH=$$clean_path( $$QSK_PLUGIN_DIR )
QSK_DIRS += $${QSK_ROOT}/skins

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

QMAKE_RPATHDIR *= $${QSK_PLUGIN_DIR}/skins
LIBS *= -L$${QSK_PLUGIN_DIR}/skins -lsquiekskin -lmaterialskin

