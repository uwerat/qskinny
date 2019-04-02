CONFIG += plugin
CONFIG += qskinny

TEMPLATE     = lib
QSK_PLUGIN_SUBDIR = skins

TARGET = $$qskPluginTarget(squiekskin)
DEFINES += QSK_SQUIEK_MAKEDLL

HEADERS += \
    QskSquiekGlobal.h \
    QskSquiekSkin.h \
    QskSquiekSkinFactory.h

SOURCES += \
    QskSquiekSkin.cpp \
    QskSquiekSkinFactory.cpp

OTHER_FILES += metadata.json

target.path    = $${QSK_INSTALL_PLUGINS}/$${QSK_PLUGIN_SUBDIR}
INSTALLS       = target
