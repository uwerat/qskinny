CONFIG += qskexample

SOURCES += \
    Box.cpp \
    BoxWithButtons.cpp \
    CircularProgressBar.cpp \
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
    UpAndDownButton.cpp \
    Usage.cpp \
    main.cpp \
    MainWindow.cpp \
    UsageDiagram.cpp

SOURCES += \
    nodes/DiagramDataNode.cpp \
    nodes/DiagramSegmentsNode.cpp

HEADERS += \
    Box.h \
    BoxWithButtons.h \
    CircularProgressBar.h \
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
    UpAndDownButton.h \
    Usage.h \
    UsageDiagram.h

HEADERS += \
    nodes/DiagramDataNode.h \
    nodes/DiagramSegmentsNode.h

HEADERS += \
src/shadowedrectangle.h \
src/scenegraph/paintedrectangleitem.h \
src/scenegraph/shadowedborderrectanglematerial.h \
src/scenegraph/shadowedrectanglematerial.h \
src/scenegraph/shadowedrectanglenode.h

SOURCES += \
src/shadowedrectangle.cpp \
src/scenegraph/paintedrectangleitem.cpp \
src/scenegraph/shadowedborderrectanglematerial.cpp \
src/scenegraph/shadowedrectanglematerial.cpp \
src/scenegraph/shadowedrectanglenode.cpp

RESOURCES += \
    images.qrc \
    fonts.qrc \
    src/scenegraph/shaders/shaders.qrc
