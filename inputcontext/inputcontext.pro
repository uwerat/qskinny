QSK_ROOT = $${PWD}/..
QSK_OUT_ROOT = $${OUT_PWD}/..

include( $${QSK_ROOT}/qskconfig.pri )

TEMPLATE = lib
TARGET = qskinputcontext

QT += quick gui-private
CONFIG += no_private_qt_headers_warning

# disable warnings for 3rdparty libs only TODO
CONFIG += warn_off

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

QMAKE_RPATHDIR *= $${QSK_OUT_ROOT}/lib
LIBS *= -L$${QSK_OUT_ROOT}/lib -lqskinny 

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
    DEFINES += QT_DLL QSK_DLL QSK_INPUTCONTEXT_MAKEDLL
}
