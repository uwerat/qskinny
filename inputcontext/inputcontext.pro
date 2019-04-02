TEMPLATE = lib
TARGET = $$qskPluginTarget(qskinputcontext)

QT += gui-private

# CONFIG += pinyin
# CONFIG += hunspell

CONFIG += plugin
CONFIG += qskinny

QSK_PLUGIN_SUBDIR = platforminputcontexts
contains(QSK_CONFIG, QskDll): DEFINES += QSK_INPUTCONTEXT_MAKEDLL

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

target.path    = $${QSK_INSTALL_PLUGINS}/$${QSK_PLUGIN_SUBDIR}
INSTALLS       = target
