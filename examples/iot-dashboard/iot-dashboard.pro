CONFIG += qskexample

SOURCES += \
    Card.cpp \
    DaytimeSkin.cpp \
    MainContent.cpp \
    MenuBar.cpp \
    PieChart.cpp \
    PieChartSkinlet.cpp \
    TopBar.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    Card.h \
    DaytimeSkin.h \
    MainContent.h \
    MainWindow.h \
    MenuBar.h \
    PieChart.h \
    PieChartSkinlet.h \
    TopBar.h

RESOURCES += \
    images.qrc \
    fonts.qrc

CONFIG -= pedantic
include(/home/peter/dev/qnanopainter/libqnanopainter/include.pri)
