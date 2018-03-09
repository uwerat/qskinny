include( $${PWD}/../playground.pri )

QMAKE_MOC_OPTIONS += -nw # disable warning about missing READ

TARGET = invoker

HEADERS += \
    Invoker.h

SOURCES += \
    Invoker.cpp \
    main.cpp
