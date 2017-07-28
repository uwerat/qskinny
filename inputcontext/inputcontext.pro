QSK_ROOT = $${PWD}/..
QSK_OUT_ROOT = $${OUT_PWD}/..

include( $${QSK_ROOT}/qskconfig.pri )

TEMPLATE = lib
TARGET = qskinputcontext

QT += quick gui-private
CONFIG += no_private_qt_headers_warning

QSK_SUBDIRS = \
    $${QSK_ROOT}/src/common \
    $${QSK_ROOT}/src/nodes \
    $${QSK_ROOT}/src/graphic \
    $${QSK_ROOT}/src/controls \
    $${QSK_ROOT}/src/layouts \
    $${QSK_ROOT}/src/dialogs

INCLUDEPATH *= $${QSK_SUBDIRS}
DEPENDPATH  += $${QSK_SUBDIRS}

DESTDIR      = $${QSK_OUT_ROOT}/plugins/platforminputcontexts

QMAKE_RPATHDIR *= $${QSK_OUT_ROOT}/lib
LIBS *= -L$${QSK_OUT_ROOT}/lib -lqskinny

win32 {
    contains(QSK_CONFIG, QskDll) {
        DEFINES += QT_DLL QSK_DLL
    }
}

SOURCES += \
    QskInputContext.cpp \
    QskInputContextPlugin.cpp \
    QskInputCompositionModel.cpp \
    QskPinyinCompositionModel.cpp \
    pinyin/zh.cpp

HEADERS += \
    QskInputContext.h \
    QskInputCompositionModel.h \
    QskPinyinCompositionModel.h \
    pinyin/zh.h

OTHER_FILES += metadata.json
