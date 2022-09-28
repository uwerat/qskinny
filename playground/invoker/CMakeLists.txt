CONFIG += qskexample

QMAKE_MOC_OPTIONS += -nw # disable warning about missing READ

list(APPEND TARGET_HEADERS
    Callback.h
    Invoker.h

list(APPEND TARGET_SOURCES
    Callback.cpp
    Invoker.cpp
    main.cpp
