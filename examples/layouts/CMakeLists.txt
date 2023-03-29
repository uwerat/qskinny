list(APPEND RESOURCES layouts.qrc)

list(APPEND HEADERS
    TestRectangle.h
    ButtonBox.h
    FlowLayoutPage.h
    GridLayoutPage.h
    LinearLayoutPage.h
    DynamicConstraintsPage.h
    StackLayoutPage.h)

list(APPEND SOURCES
    TestRectangle.cpp
    ButtonBox.cpp
    FlowLayoutPage.cpp
    GridLayoutPage.cpp
    LinearLayoutPage.cpp
    DynamicConstraintsPage.cpp
    StackLayoutPage.cpp
    main.cpp)

qsk_example(layouts)
