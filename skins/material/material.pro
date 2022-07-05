CONFIG += plugin
CONFIG += qskinny

TEMPLATE = lib
QSK_PLUGIN_SUBDIR = skins

TARGET = $$qskPluginTarget(materialskin)
DEFINES += QSK_MATERIAL_MAKEDLL

HEADERS += \
    QskMaterialGlobal.h \
    QskMaterialSkin.h \
    QskMaterialSkinFactory.h

SOURCES += \
    QskMaterialSkin.cpp \
    QskMaterialSkinFactory.cpp

OTHER_FILES += metadata.json

target.path    = $${QSK_INSTALL_PLUGINS}/$${QSK_PLUGIN_SUBDIR}
INSTALLS       = target

