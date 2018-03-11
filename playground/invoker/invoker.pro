include( $${PWD}/../playground.pri )

QMAKE_MOC_OPTIONS += -nw # disable warning about missing READ

TARGET = invoker

HEADERS += \
    Callback.h \
    Invoker.h

SOURCES += \
    Callback.cpp \
    Invoker.cpp \
    main.cpp
