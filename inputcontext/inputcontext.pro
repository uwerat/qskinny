QSK_ROOT = $${PWD}/..
QSK_OUT_ROOT = $${OUT_PWD}/..

include( $${QSK_ROOT}/qskconfig.pri )

TEMPLATE = lib
TARGET = qskinputcontext

QT += quick gui-private
CONFIG += no_private_qt_headers_warning

# disable warnings for 3rdparty libs only TODO
CONFIG += warn_off

CONFIG += pinyin
CONFIG += hunspell

QSK_SUBDIRS = \
    $${QSK_ROOT}/src/common \
    $${QSK_ROOT}/src/nodes \
    $${QSK_ROOT}/src/graphic \
    $${QSK_ROOT}/src/controls \
    $${QSK_ROOT}/src/layouts \
    $${QSK_ROOT}/src/dialogs \
    $${QSK_ROOT}/src/inputpanel

INCLUDEPATH *= $${QSK_SUBDIRS}
DEPENDPATH  *= $${QSK_SUBDIRS}

DESTDIR      = $${QSK_OUT_ROOT}/plugins/platforminputcontexts

QMAKE_RPATHDIR *= $${QSK_OUT_ROOT}/lib
LIBS *= -L$${QSK_OUT_ROOT}/lib -lqskinny 

SOURCES += \
    QskInputContextPlugin.cpp

OTHER_FILES += metadata.json

pinyin {

    QSK_PINYIN_DIR = 3rdparty/pinyin
    INCLUDEPATH *= $${QSK_PINYIN_DIR}/include

    SOURCES += \
        $${QSK_PINYIN_DIR}/share/dictbuilder.cpp \
        $${QSK_PINYIN_DIR}/share/dictlist.cpp \
        $${QSK_PINYIN_DIR}/share/dicttrie.cpp \
        $${QSK_PINYIN_DIR}/share/lpicache.cpp \
        $${QSK_PINYIN_DIR}/share/matrixsearch.cpp \
        $${QSK_PINYIN_DIR}/share/mystdlib.cpp \
        $${QSK_PINYIN_DIR}/share/ngram.cpp \
        $${QSK_PINYIN_DIR}/share/pinyinime.cpp \
        $${QSK_PINYIN_DIR}/share/searchutility.cpp \
        $${QSK_PINYIN_DIR}/share/spellingtable.cpp \
        $${QSK_PINYIN_DIR}/share/spellingtrie.cpp \
        $${QSK_PINYIN_DIR}/share/splparser.cpp \
        $${QSK_PINYIN_DIR}/share/sync.cpp \
        $${QSK_PINYIN_DIR}/share/userdict.cpp \
        $${QSK_PINYIN_DIR}/share/utf16char.cpp \
        $${QSK_PINYIN_DIR}/share/utf16reader.cpp

    HEADERS += \
        $${QSK_PINYIN_DIR}/include/atomdictbase.h \
        $${QSK_PINYIN_DIR}/include/dictbuilder.h \
        $${QSK_PINYIN_DIR}/include/dictdef.h \
        $${QSK_PINYIN_DIR}/include/dictlist.h \
        $${QSK_PINYIN_DIR}/include/dicttrie.h \
        $${QSK_PINYIN_DIR}/include/lpicache.h \
        $${QSK_PINYIN_DIR}/include/matrixsearch.h \
        $${QSK_PINYIN_DIR}/include/mystdlib.h \
        $${QSK_PINYIN_DIR}/include/ngram.h \
        $${QSK_PINYIN_DIR}/include/pinyinime.h \
        $${QSK_PINYIN_DIR}/include/searchutility.h \
        $${QSK_PINYIN_DIR}/include/spellingtable.h \
        $${QSK_PINYIN_DIR}/include/spellingtrie.h \
        $${QSK_PINYIN_DIR}/include/splparser.h \
        $${QSK_PINYIN_DIR}/include/sync.h \
        $${QSK_PINYIN_DIR}/include/userdict.h \
        $${QSK_PINYIN_DIR}/include/utf16char.h \
        $${QSK_PINYIN_DIR}/include/utf16reader.h

    HEADERS += \
         QskPinyinCompositionModel.h

    SOURCES += \
         QskPinyinCompositionModel.cpp
}

hunspell {

    QSK_HUNSPELL_DIR = 3rdparty/hunspell
    INCLUDEPATH *= $${QSK_HUNSPELL_DIR}

    HEADERS += \
        $${QSK_HUNSPELL_DIR}/affentry.hxx \
        $${QSK_HUNSPELL_DIR}/affixmgr.hxx \
        $${QSK_HUNSPELL_DIR}/atypes.hxx \
        $${QSK_HUNSPELL_DIR}/baseaffix.hxx \
        $${QSK_HUNSPELL_DIR}/csutil.hxx \
        $${QSK_HUNSPELL_DIR}/filemgr.hxx \
        $${QSK_HUNSPELL_DIR}/hashmgr.hxx \
        $${QSK_HUNSPELL_DIR}/htypes.hxx \
        $${QSK_HUNSPELL_DIR}/hunspell.h \
        $${QSK_HUNSPELL_DIR}/hunspell.hxx \
        $${QSK_HUNSPELL_DIR}/hunvisapi.h \
        $${QSK_HUNSPELL_DIR}/hunzip.hxx \
        $${QSK_HUNSPELL_DIR}/langnum.hxx \
        $${QSK_HUNSPELL_DIR}/phonet.hxx \
        $${QSK_HUNSPELL_DIR}/replist.hxx \
        $${QSK_HUNSPELL_DIR}/suggestmgr.hxx \
        $${QSK_HUNSPELL_DIR}/w_char.hxx

    SOURCES += \
        $${QSK_HUNSPELL_DIR}/affentry.cxx \
        $${QSK_HUNSPELL_DIR}/affixmgr.cxx \
        $${QSK_HUNSPELL_DIR}/csutil.cxx \
        $${QSK_HUNSPELL_DIR}/filemgr.cxx \
        $${QSK_HUNSPELL_DIR}/hashmgr.cxx \
        $${QSK_HUNSPELL_DIR}/hunspell.cxx \
        $${QSK_HUNSPELL_DIR}/hunzip.cxx \
        $${QSK_HUNSPELL_DIR}/phonet.cxx \
        $${QSK_HUNSPELL_DIR}/replist.cxx \
        $${QSK_HUNSPELL_DIR}/suggestmgr.cxx

    HEADERS += \
        QskHunspellCompositionModel.h

    SOURCES += \
        QskHunspellCompositionModel.cpp

    OTHER_FILES +=\
        $${QSK_HUNSPELL_DIR}/license.hunspell \
        $${QSK_HUNSPELL_DIR}/license.myspell
}

win32 {
    contains(QSK_CONFIG, QskDll) {
        DEFINES += QT_DLL QSK_DLL
    }
}
