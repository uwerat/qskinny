QSK_OUT_ROOT = $${OUT_PWD}/../..
QSK_PLUGIN_DIR = $${QSK_OUT_ROOT}/plugins

TEMPLATE = lib
TARGET = qskinputcontext_hunspell

CONFIG += static
CONFIG += precompile_header warn_off

MODULE_INCLUDEPATH = $$PWD/src

DESTDIR      = $${QSK_OUT_ROOT}/plugins/platforminputcontexts
QMAKE_RPATHDIR *= $${DESTDIR_LIBS}

SOURCES += \
    src/hunspell/affentry.cxx \
    src/hunspell/affixmgr.cxx \
    src/hunspell/csutil.cxx \
    src/hunspell/filemgr.cxx \
    src/hunspell/hashmgr.cxx \
    src/hunspell/hunspell.cxx \
    src/hunspell/hunzip.cxx \
    src/hunspell/phonet.cxx \
    src/hunspell/replist.cxx \
    src/hunspell/suggestmgr.cxx

HEADERS += \
    src/hunspell/affentry.hxx \
    src/hunspell/affixmgr.hxx \
    src/hunspell/atypes.hxx \
    src/hunspell/baseaffix.hxx \
    src/hunspell/csutil.hxx \
    src/hunspell/filemgr.hxx \
    src/hunspell/hashmgr.hxx \
    src/hunspell/htypes.hxx \
    src/hunspell/hunspell.h \
    src/hunspell/hunspell.hxx \
    src/hunspell/hunvisapi.h \
    src/hunspell/hunzip.hxx \
    src/hunspell/langnum.hxx \
    src/hunspell/phonet.hxx \
    src/hunspell/replist.hxx \
    src/hunspell/suggestmgr.hxx \
    src/hunspell/w_char.hxx

OTHER_FILES +=\
    src/hunspell/license.hunspell \
    src/hunspell/license.myspell \
    src/hunspell/utf_info.cxx
