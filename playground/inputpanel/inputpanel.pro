include( $${PWD}/../playground.pri )

TARGET = inputpanel

DEFINES += PLUGIN_PATH=$$clean_path( $$QSK_OUT_ROOT/plugins )

SOURCES += \
    main.cpp \
    TextInput.cpp

HEADERS += \
    TextInput.h
