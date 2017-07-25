include( $${PWD}/../examples.pri )

TARGET = hmi-demo

RESOURCES = images.qrc

HEADERS += \
    MainWindow.h \
    RadioControl.h \
    SoundControl.h \
    DefaultSkin.h \
    OtherSkin.h

SOURCES += \
    main.cpp \
    MainWindow.cpp \
    RadioControl.cpp \
    SoundControl.cpp \
    DefaultSkin.cpp \
    OtherSkin.cpp
