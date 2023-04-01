############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
# This file may be used under the terms of the 3-clause BSD License
############################################################################

set(SOURCES
    TestRectangle.h TestRectangle.cpp
    ButtonBox.h ButtonBox.cpp
    FlowLayoutPage.h FlowLayoutPage.cpp
    GridLayoutPage.h GridLayoutPage.cpp
    LinearLayoutPage.h LinearLayoutPage.cpp
    DynamicConstraintsPage.h DynamicConstraintsPage.cpp
    StackLayoutPage.h StackLayoutPage.cpp
    main.cpp
)
qt_add_resources(SOURCES layouts.qrc)

qsk_add_example(layouts ${SOURCES})
