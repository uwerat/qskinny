CONFIG += qskexample

QT += svg

SOURCES += \
    Box.cpp \
    BoxWithButtons.cpp \
    CircularProgressBar.cpp \
    CircularProgressBarSkinlet.cpp \
    Diagram.cpp \
    DiagramSkinlet.cpp \
    GraphicProvider.cpp \
    LightIntensity.cpp \
    MainContent.cpp \
    MenuBar.cpp \
    MyDevices.cpp \
    PieChart.cpp \
    PieChartPainted.cpp \
    PieChartSkinlet.cpp \
    RoundedIcon.cpp \
    ShadowedBox.cpp \
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
    nodes/BoxShadowNode.cpp

HEADERS += \
    Box.h \
    BoxWithButtons.h \
    CircularProgressBar.h \
    CircularProgressBarSkinlet.h \
    Diagram.h \
    DiagramSkinlet.h \
    GraphicProvider.h \
    LightIntensity.h \
    MainContent.h \
    MainWindow.h \
    MenuBar.h \
    MyDevices.h \
    PieChart.h \
    PieChartPainted.h \
    PieChartSkinlet.h \
    RoundedIcon.h \
    ShadowedBox.h \
    Skin.h \
    TopBar.h \
    RoundButton.h \
    UsageBox.h \
    UsageDiagram.h

HEADERS += \
    nodes/DiagramDataNode.h \
    nodes/DiagramSegmentsNode.h \
    nodes/BoxShadowNode.h

RESOURCES += \
    images.qrc \
    fonts.qrc \
    shaders.qrc
