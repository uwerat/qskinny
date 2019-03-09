include( $${PWD}/../skins.pri )

TARGET = $$qskPluginTarget(squiekskin)

HEADERS += \
    QskSquiekGlobal.h \
    QskSquiekSkin.h \
    QskSquiekSkinFactory.h

SOURCES += \
    QskSquiekSkin.cpp \
    QskSquiekSkinFactory.cpp

OTHER_FILES += metadata.json

DEFINES += QSK_SQUIEK_MAKEDLL
