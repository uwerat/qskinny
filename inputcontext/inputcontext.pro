SQUIEK_ROOT = $${PWD}/..
SQUIEK_OUT_ROOT = $${OUT_PWD}/..

include( $${SQUIEK_ROOT}/qskconfig.pri )

TEMPLATE = lib
TARGET = skinnyinputcontext

QT += quick gui-private
CONFIG += no_private_qt_headers_warning

SQUIEK_DIRS = \
    $${SQUIEK_ROOT}/src/common \
    $${SQUIEK_ROOT}/src/nodes \
    $${SQUIEK_ROOT}/src/graphic \
    $${SQUIEK_ROOT}/src/controls \
    $${SQUIEK_ROOT}/src/layouts \
    $${SQUIEK_ROOT}/src/dialogs

INCLUDEPATH *= $${SQUIEK_DIRS}
DEPENDPATH  += $${SQUIEK_DIRS}

DESTDIR      = $${SQUIEK_OUT_ROOT}/examples/bin/platforminputcontexts

QMAKE_RPATHDIR *= $${SQUIEK_OUT_ROOT}/lib
LIBS *= -L$${SQUIEK_OUT_ROOT}/lib -lqskinny

win32: contains(SQUIEK_CONFIG, SquiekDll): DEFINES += QT_DLL QSK_DLL SQUIEK_DLL

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

OTHER_FILES += metadata.json qml/ANSPInputPanel.qml
