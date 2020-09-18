CONFIG += qskexample

SOURCES += \
    Box.cpp \
    CircularProgressBar.cpp \
    DaytimeSkin.cpp \
    Diagram.cpp \
    Humidity.cpp \
    IndoorTemperature.cpp \
    LightIntensity.cpp \
    MainContent.cpp \
    MenuBar.cpp \
    MyDevices.cpp \
    PieChart.cpp \
    PieChartPainted.cpp \
    PieChartSkinlet.cpp \
    QskShadowedRectangle.cpp \
    QskShadowedRectangleSkinlet.cpp \
    RoundedIcon.cpp \
    TopBar.cpp \
    Usage.cpp \
    main.cpp \
    MainWindow.cpp

HEADERS += \
    Box.h \
    CircularProgressBar.h \
    DaytimeSkin.h \
    Diagram.h \
    Humidity.h \
    IndoorTemperature.h \
    LightIntensity.h \
    MainContent.h \
    MainWindow.h \
    MenuBar.h \
    MyDevices.h \
    PieChart.h \
    PieChartPainted.h \
    PieChartSkinlet.h \
    QskShadowedRectangle.h \
    QskShadowedRectangleSkinlet.h \
    RoundedIcon.h \
    TopBar.h \
    Usage.h

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
