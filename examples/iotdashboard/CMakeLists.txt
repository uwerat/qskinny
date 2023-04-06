############################################################################
# Copyright (C) 2021 Edelhirsch Software GmbH
#           SPDX-License-Identifier: BSD-3-Claus
############################################################################

set(SOURCES
    Box.h Box.cpp
    BoxWithButtons.h BoxWithButtons.cpp
    CircularProgressBar.h CircularProgressBar.cpp
    CircularProgressBarSkinlet.h CircularProgressBarSkinlet.cpp
    Diagram.h Diagram.cpp
    DiagramSkinlet.h DiagramSkinlet.cpp
    EnergyMeter.h EnergyMeter.cpp
    GraphicProvider.h GraphicProvider.cpp
    GridBox.h GridBox.cpp
    LightDisplaySkinlet.h LightDisplaySkinlet.cpp
    LightDisplay.h LightDisplay.cpp
    DashboardPage.h DashboardPage.cpp
    DevicesPage.h DevicesPage.cpp
    MainItem.h MainItem.cpp
    MainWindow.h MainWindow.cpp
    MembersPage.h MembersPage.cpp
    MenuBar.h MenuBar.cpp
    MyDevices.h MyDevices.cpp
    RoomsPage.h RoomsPage.cpp
    RoundedIcon.h RoundedIcon.cpp
    Skin.h Skin.cpp
    StatisticsPage.h StatisticsPage.cpp
    TopBar.h TopBar.cpp
    RoundButton.h RoundButton.cpp
    UsageBox.h UsageBox.cpp
    UsageDiagram.h UsageDiagram.cpp
    StoragePage.h StoragePage.cpp
    StorageMeter.h StorageMeter.cpp
    StorageBar.h StorageBar.cpp
    StorageBarSkinlet.h StorageBarSkinlet.cpp
    nodes/DiagramDataNode.h nodes/DiagramDataNode.cpp
    nodes/DiagramSegmentsNode.h nodes/DiagramSegmentsNode.cpp
    nodes/RadialTickmarksNode.cpp nodes/RadialTickmarksNode.h
    main.cpp
)
qt_add_resources(SOURCES images.qrc fonts.qrc)

qsk_add_example(iotdashboard ${SOURCES})
