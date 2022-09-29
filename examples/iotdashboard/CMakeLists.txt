list(APPEND TARGET_SOURCES
    Box.cpp
    BoxWithButtons.cpp
    CircularProgressBar.cpp
    CircularProgressBarSkinlet.cpp
    DashboardPage.cpp
    Diagram.cpp
    DiagramSkinlet.cpp
    GraphicProvider.cpp
    GridBox.cpp
    LightDisplaySkinlet.cpp
    LightDisplay.cpp
    MainItem.cpp
    MenuBar.cpp
    MyDevices.cpp
    PieChart.cpp
    PieChartPainted.cpp
    PieChartSkinlet.cpp
    RoomsPage.cpp
    RoundedIcon.cpp
    Skin.cpp
    TopBar.cpp
    RoundButton.cpp
    UsageBox.cpp
    UsageDiagram.cpp
    MainWindow.cpp
    main.cpp)

list(APPEND TARGET_SOURCES
    nodes/DiagramDataNode.cpp
    nodes/DiagramSegmentsNode.cpp
    nodes/RadialTickmarksNode.cpp)

list(APPEND TARGET_HEADERS
    Box.h
    BoxWithButtons.h
    CircularProgressBar.h
    CircularProgressBarSkinlet.h
    Diagram.h
    DiagramSkinlet.h
    GraphicProvider.h
    GridBox.h
    LightDisplaySkinlet.h
    LightDisplay.h
    DashboardPage.h
    MainItem.h
    MainWindow.h
    MenuBar.h
    MyDevices.h
    PieChart.h
    PieChartPainted.h
    PieChartSkinlet.h
    RoomsPage.h
    RoundedIcon.h
    Skin.h
    TopBar.h
    RoundButton.h
    UsageBox.h
    UsageDiagram.h)

list(APPEND TARGET_HEADERS
    nodes/DiagramDataNode.h
    nodes/DiagramSegmentsNode.h
    nodes/RadialTickmarksNode.h)

list(APPEND TARGET_RESSOURCES
    images.qrc
    fonts.qrc)

set(TARGET_NAME iotdashboard)

add_executable(${TARGET_NAME}
    ${TARGET_SOURCES}
    ${TARGET_HEADERS}
    ${TARGET_RESSOURCES})

set_target_properties(${TARGET_NAME}
    PROPERTIES
        AUTOMOC ON
        AUTORCC ON
        FOLDER examples)

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        qsktestsupport
        Qt5::Svg)