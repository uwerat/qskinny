include( $${PWD}/../playground.pri )

TARGET = lineedit

RESOURCES += \
    qml.qrc

HEADERS += \
    LineEditSkinlet.h \
    LineEdit.h

SOURCES += \
    LineEditSkinlet.cpp \
    LineEdit.cpp \
    main.cpp
