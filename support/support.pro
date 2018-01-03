QSK_ROOT = $${PWD}/..
QSK_OUT_ROOT = $${OUT_PWD}/..

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

INCLUDEPATH *= $${QSK_DIRS}
DEPENDPATH  += $${QSK_DIRS}

HEADERS += \
    SkinnyGlobal.h \
    SkinnyFont.h \
    SkinnyShapeFactory.h \
    SkinnyShapeProvider.h \
    SkinnyShortcut.h

SOURCES += \
    SkinnyFont.cpp \
    SkinnyShapeFactory.cpp \
    SkinnyShapeProvider.cpp \
    SkinnyShortcut.cpp

RESOURCES += \
    fonts.qrc 
