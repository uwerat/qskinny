CONFIG += plugin
CONFIG += qskinny

TEMPLATE = lib
QSK_PLUGIN_SUBDIR = skins

TARGET = $$qskPluginTarget(material3skin)
DEFINES += QSK_MATERIAL3_MAKEDLL

HEADERS += \
    QskMaterial3Global.h \
    QskMaterial3Skin.h \
    QskMaterial3SkinFactory.h

SOURCES += \
    QskMaterial3Skin.cpp \
    QskMaterial3SkinFactory.cpp

OTHER_FILES += metadata.json

target.path    = $${QSK_INSTALL_PLUGINS}/$${QSK_PLUGIN_SUBDIR}
INSTALLS       = target

