CONFIG += qskexample

SOURCES += \
    Box.cpp \
    BoxWithButtons.cpp \
    CircularProgressBar.cpp \
    CircularProgressBarSkinlet.cpp \
    Diagram.cpp \
    DiagramSkinlet.cpp \
    LightIntensity.cpp \
    MainContent.cpp \
    MenuBar.cpp \
    MyDevices.cpp \
    PieChart.cpp \
    PieChartPainted.cpp \
    PieChartSkinlet.cpp \
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
    nodes/DiagramSegmentsNode.cpp

HEADERS += \
    Box.h \
    BoxWithButtons.h \
    CircularProgressBar.h \
    CircularProgressBarSkinlet.h \
    Diagram.h \
    DiagramSkinlet.h \
    LightIntensity.h \
    MainContent.h \
    MainWindow.h \
    MenuBar.h \
    MyDevices.h \
    PieChart.h \
    PieChartPainted.h \
    PieChartSkinlet.h \
    RoundedIcon.h \
    Skin.h \
    TopBar.h \
    RoundButton.h \
    UsageBox.h \
    UsageDiagram.h

HEADERS += \
    nodes/DiagramDataNode.h \
    nodes/DiagramSegmentsNode.h

HEADERS += \
    kirigami/shadowedrectangle.h \
    kirigami/scenegraph/paintedrectangleitem.h \
    kirigami/scenegraph/shadowedborderrectanglematerial.h \
    kirigami/scenegraph/shadowedrectanglematerial.h \
    kirigami/scenegraph/shadowedrectanglenode.h

SOURCES += \
    kirigami/shadowedrectangle.cpp \
    kirigami/scenegraph/paintedrectangleitem.cpp \
    kirigami/scenegraph/shadowedborderrectanglematerial.cpp \
    kirigami/scenegraph/shadowedrectanglematerial.cpp \
    kirigami/scenegraph/shadowedrectanglenode.cpp

RESOURCES += \
    images.qrc \
    fonts.qrc \
    kirigami/scenegraph/shaders/shaders.qrc
