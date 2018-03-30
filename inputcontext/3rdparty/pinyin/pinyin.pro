QSK_OUT_ROOT = $${OUT_PWD}/../..
QSK_PLUGIN_DIR = $${QSK_OUT_ROOT}/plugins

TARGET = qskinputcontext_pinyin
TEMPLATE = lib

VERSION = 1.0.0
#CONFIG += qt
CONFIG += static
CONFIG += warn_off

MODULE_INCLUDEPATH = $$PWD/include

DESTDIR      = $${QSK_OUT_ROOT}/plugins/platforminputcontexts
QMAKE_RPATHDIR *= $${DESTDIR_LIBS}

SOURCES += \
    share/dictbuilder.cpp \
    share/dictlist.cpp \
    share/dicttrie.cpp \
    share/lpicache.cpp \
    share/matrixsearch.cpp \
    share/mystdlib.cpp \
    share/ngram.cpp \
    share/pinyinime.cpp \
    share/searchutility.cpp \
    share/spellingtable.cpp \
    share/spellingtrie.cpp \
    share/splparser.cpp \
    share/sync.cpp \
    share/userdict.cpp \
    share/utf16char.cpp \
    share/utf16reader.cpp

HEADERS += \
    include/atomdictbase.h \
    include/dictbuilder.h \
    include/dictdef.h \
    include/dictlist.h \
    include/dicttrie.h \
    include/lpicache.h \
    include/matrixsearch.h \
    include/mystdlib.h \
    include/ngram.h \
    include/pinyinime.h \
    include/searchutility.h \
    include/spellingtable.h \
    include/spellingtrie.h \
    include/splparser.h \
    include/sync.h \
    include/userdict.h \
    include/utf16char.h \
    include/utf16reader.h

OTHER_FILES +=\
    data/rawdict_utf16_65105_freq.txt \
    data/valid_utf16.txt

pinyin_dict.files = data/dict_pinyin.dat
pinyin_dict.path = $${INSTALLDIR}/3rdparty/pinyin/data/
INSTALLS += pinyin_dict
