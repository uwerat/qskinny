CONFIG += qskexample

QT += svg
QT += quick_private # TODO: examples should not use private headers

SOURCES += \
    Box.cpp \
    BoxWithButtons.cpp \
    CircularProgressBar.cpp \
    CircularProgressBarSkinlet.cpp \
    DashboardPage.cpp \
    Diagram.cpp \
    DiagramSkinlet.cpp \
    GraphicProvider.cpp \
    GridBox.cpp \
    LightDisplaySkinlet.cpp \
    LightDisplay.cpp \
    MainItem.cpp \
    MenuBar.cpp \
    MyDevices.cpp \
    PieChart.cpp \
    PieChartPainted.cpp \
    PieChartSkinlet.cpp \
    RoomsPage.cpp \
    RoundedIcon.cpp \
    Skin.cpp \
    TopBar.cpp \
    RoundButton.cpp \
    UsageBox.cpp \
    UsageDiagram.cpp \
    MainWindow.cpp \
    main.cpp \

SOURCES += \
    nodes/DiagramDataNode.cpp \
    nodes/DiagramSegmentsNode.cpp \
    nodes/RadialTickmarksNode.cpp

HEADERS += \
    Box.h \
    BoxWithButtons.h \
    CircularProgressBar.h \
    CircularProgressBarSkinlet.h \
    Diagram.h \
    DiagramSkinlet.h \
    GraphicProvider.h \
    GridBox.h \
    LightDisplaySkinlet.h \
    LightDisplay.h \
    DashboardPage.h \
    MainItem.h \
    MainWindow.h \
    MenuBar.h \
    MyDevices.h \
    PieChart.h \
    PieChartPainted.h \
    PieChartSkinlet.h \
    RoomsPage.h \
    RoundedIcon.h \
    Skin.h \
    TopBar.h \
    RoundButton.h \
    UsageBox.h \
    UsageDiagram.h

HEADERS += \
    nodes/DiagramDataNode.h \
    nodes/DiagramSegmentsNode.h \
    nodes/RadialTickmarksNode.h

RESOURCES += \
    images.qrc \
    fonts.qrc
