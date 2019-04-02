CONFIG += qskexample

QMAKE_MOC_OPTIONS += -nw # disable warning about missing READ

HEADERS += \
    Callback.h \
    Invoker.h

SOURCES += \
    Callback.cpp \
    Invoker.cpp \
    main.cpp
