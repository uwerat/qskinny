CONFIG += qskexample qskqmlexport

list(APPEND TARGET_RESSOURCES
    layouts.qrc

list(APPEND TARGET_HEADERS
    TestRectangle.h
    ButtonBox.h
    FlowLayoutPage.h
    GridLayoutPage.h
    LinearLayoutPage.h
    DynamicConstraintsPage.h
    StackLayoutPage.h

list(APPEND TARGET_SOURCES
    TestRectangle.cpp
    ButtonBox.cpp
    FlowLayoutPage.cpp
    GridLayoutPage.cpp
    LinearLayoutPage.cpp
    DynamicConstraintsPage.cpp
    StackLayoutPage.cpp
    main.cpp
