include( $${PWD}/../examples.pri )

TARGET = layouts

QT += quick

HEADERS += \
    TestRectangle.h \
    ButtonBox.h \
    FlowLayoutPage.h \
    LinearLayoutPage.h \
    DynamicConstraintsPage.h \
    StackLayoutPage.h

SOURCES += \
    TestRectangle.cpp \
    ButtonBox.cpp \
    FlowLayoutPage.cpp \
    LinearLayoutPage.cpp \
    DynamicConstraintsPage.cpp \
    StackLayoutPage.cpp \
    main.cpp
