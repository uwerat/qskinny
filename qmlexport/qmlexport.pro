QSK_ROOT = $${PWD}/..
QSK_OUT_ROOT = $${OUT_PWD}/..

include( $${QSK_ROOT}/qskconfig.pri )

TEMPLATE = lib
TARGET   = qskqmlexport

DESTDIR  = $${QSK_OUT_ROOT}/lib

QT += quick quick-private
CONFIG += no_private_qt_headers_warning

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
    $${QSK_ROOT}/src/dialogs \
    $${QSK_ROOT}/src/inputpanel \
    $${QSK_ROOT}/src/layouts \
    $${QSK_ROOT}/src/graphic 

INCLUDEPATH *= $${QSK_DIRS}
DEPENDPATH  *= $${QSK_DIRS}

HEADERS += \
    QskShortcut.h \
    QskQml.h

SOURCES += \
    QskShortcut.cpp \
    QskQml.cpp

