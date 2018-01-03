include( $${PWD}/../skins.pri )

TARGET = materialskin

HEADERS += \
    QskMaterialSkin.h \
    QskMaterialSkinFactory.h

SOURCES += \
    QskMaterialSkin.cpp \
    QskMaterialSkinFactory.cpp

OTHER_FILES += metadata.json
