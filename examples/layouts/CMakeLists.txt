############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES
    TestRectangle.h TestRectangle.cpp
    ButtonBox.h ButtonBox.cpp
    FlowLayoutPage.h FlowLayoutPage.cpp
    GridLayoutPage.h GridLayoutPage.cpp
    LinearLayoutPage.h LinearLayoutPage.cpp
    DynamicConstraintsPage.h DynamicConstraintsPage.cpp
    StackLayoutPage.h StackLayoutPage.cpp
    SwipeViewPage.h SwipeViewPage.cpp
    main.cpp
)
qt_add_resources(SOURCES layouts.qrc)

qsk_add_example(layouts ${SOURCES})
