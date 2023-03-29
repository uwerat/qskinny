list(APPEND TARGET_SOURCES
    Box.cpp
    BoxWithButtons.cpp
    CircularProgressBar.cpp
    CircularProgressBarSkinlet.cpp
    DashboardPage.cpp
    DevicesPage.cpp
    Diagram.cpp
    DiagramSkinlet.cpp
    EnergyMeter.cpp
    GraphicProvider.cpp
    GridBox.cpp
    LightDisplaySkinlet.cpp
    LightDisplay.cpp
    MainItem.cpp
    MainWindow.cpp
    MenuBar.cpp
    MembersPage.cpp
    MyDevices.cpp
    RoomsPage.cpp
    RoundedIcon.cpp
    Skin.cpp
    StatisticsPage.cpp
    TopBar.cpp
    RoundButton.cpp
    UsageBox.cpp
    UsageDiagram.cpp
    StoragePage.cpp
    StorageMeter.cpp
    StorageBar.cpp
    StorageBarSkinlet.cpp
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
    EnergyMeter.h
    GraphicProvider.h
    GridBox.h
    LightDisplaySkinlet.h
    LightDisplay.h
    DashboardPage.h
    DevicesPage.h
    MainItem.h
    MainWindow.h
    MembersPage.h
    MenuBar.h
    MyDevices.h
    RoomsPage.h
    RoundedIcon.h
    Skin.h
    StatisticsPage.h
    TopBar.h
    RoundButton.h
    UsageBox.h
    UsageDiagram.h
    StoragePage.h
    StorageMeter.h
    StorageBar.h
    StorageBarSkinlet.h)

list(APPEND TARGET_HEADERS
    nodes/DiagramDataNode.h
    nodes/DiagramSegmentsNode.h
    nodes/RadialTickmarksNode.h)

list(APPEND TARGET_RESSOURCES
    images.qrc
    fonts.qrc)

set(TARGET_NAME iotdashboard)

qt_add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE
    ${TARGET_SOURCES}
    ${TARGET_HEADERS}
    ${TARGET_RESSOURCES})

if(TARGET Qsk::qskinny) 
    # integration test
    # - qsktestsupport doesn't exists
    # - qskinny has a namespace Qsk
    target_link_libraries(${TARGET_NAME}
        PRIVATE
            Qsk::qskinny)
else()
    target_link_libraries(${TARGET_NAME}
        PRIVATE
            qskinny
            qsktestsupport)

    target_compile_definitions(${TARGET_NAME}
        PRIVATE
            USE_SHORTCUTS)
endif()

include(${CMAKE_SOURCE_DIR}/cmake/QskExamples.cmake)
