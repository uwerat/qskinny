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
DEPENDPATH  *= $${QSK_SUBDIRS}

DESTDIR      = $${QSK_OUT_ROOT}/plugins/platforminputcontexts

QMAKE_RPATHDIR *= $${QSK_OUT_ROOT}/lib
LIBS *= -L$${QSK_OUT_ROOT}/lib -lqskinny 

# we need to write the lines below in a platform independent way: TODO ...

pinyin {
    INCLUDEPATH *= $${QSK_ROOT}/3rdparty/pinyin/include
    LIBS *= -Wl,-Bstatic -L$${DESTDIR} -lqskinputcontext_pinyin
}

hunspell {
    INCLUDEPATH *= $${QSK_ROOT}/3rdparty/hunspell/src
    LIBS *= -Wl,-Bstatic -L$${DESTDIR} -lqskinputcontext_hunspell
}

LIBS *= -Wl,-Bdynamic

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
    QskHunspellCompositionModel.cpp

HEADERS += \
    QskInputContext.h \
    QskInputCompositionModel.h \
    QskPinyinCompositionModel.h \
    QskHunspellCompositionModel.h

OTHER_FILES += metadata.json
