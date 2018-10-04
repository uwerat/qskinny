include( $${PWD}/../example.pri )
include( $${PWD}/../qmlexample.pri )

TARGET = colorswitch

RESOURCES += \
    colorswitch.qrc

HEADERS += \
    Theme.h

SOURCES += \
    Theme.cpp \
    main.cpp
