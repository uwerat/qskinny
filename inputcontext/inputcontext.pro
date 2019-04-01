QSK_ROOT = $${PWD}/..
QSK_OUT_ROOT = $${OUT_PWD}/..

TEMPLATE = lib
TARGET = $$qskPluginTarget(qskinputcontext)

QT += quick gui-private
CONFIG += no_private_qt_headers_warning

# CONFIG += pinyin
# CONFIG += hunspell

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

qskAddLibrary($${QSK_OUT_ROOT}/lib, qskinny)

HEADERS += \
    QskInputContextGlobal.h

SOURCES += \
    QskInputContextPlugin.cpp

OTHER_FILES += metadata.json

pinyin {

    unix {
    
        CONFIG += link_pkgconfig
        PKGCONFIG += pinyin

        HEADERS += \
            QskPinyinTextPredictor.h

        SOURCES += \
            QskPinyinTextPredictor.cpp
    }
}

hunspell {

    unix {

        CONFIG += link_pkgconfig
        PKGCONFIG += hunspell

        HEADERS += \
            QskHunspellTextPredictor.h

        SOURCES += \
            QskHunspellTextPredictor.cpp
    }

}

contains(QSK_CONFIG, QskDll) {
    DEFINES += QSK_DLL QSK_INPUTCONTEXT_MAKEDLL
}

target.path    = $${QSK_INSTALL_PLUGINS}/platforminputcontexts
INSTALLS       = target

