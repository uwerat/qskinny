include( $${PWD}/../playground.pri )
include( $${PWD}/../qmlplayground.pri )

TARGET = images

RESOURCES += \
    images.qrc

HEADERS += \
    Image.h

SOURCES += \
    Image.cpp \
    main.cpp
