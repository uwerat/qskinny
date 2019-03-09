include( $${PWD}/../skins.pri )

TARGET = $$qskPluginTarget(materialskin)

HEADERS += \
    QskMaterialGlobal.h \
    QskMaterialSkin.h \
    QskMaterialSkinFactory.h

SOURCES += \
    QskMaterialSkin.cpp \
    QskMaterialSkinFactory.cpp

OTHER_FILES += metadata.json

DEFINES += QSK_MATERIAL_MAKEDLL
