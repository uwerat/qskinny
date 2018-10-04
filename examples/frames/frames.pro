include( $${PWD}/../example.pri )
include( $${PWD}/../qmlexample.pri )

TARGET = frames

RESOURCES += \
    frames.qrc

HEADERS += \
    Frame.h

SOURCES += \
    Frame.cpp \
    main.cpp
