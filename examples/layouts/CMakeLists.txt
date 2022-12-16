list(APPEND TARGET_RESSOURCES
    layouts.qrc)

list(APPEND TARGET_HEADERS
    TestRectangle.h
    ButtonBox.h
    FlowLayoutPage.h
    GridLayoutPage.h
    LinearLayoutPage.h
    DynamicConstraintsPage.h
    StackLayoutPage.h)

list(APPEND TARGET_SOURCES
    TestRectangle.cpp
    ButtonBox.cpp
    FlowLayoutPage.cpp
    GridLayoutPage.cpp
    LinearLayoutPage.cpp
    DynamicConstraintsPage.cpp
    StackLayoutPage.cpp
    main.cpp)

set(TARGET_NAME layouts)

add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE ${TARGET_SOURCES} ${TARGET_HEADERS} ${TARGET_RESSOURCES})

set_target_properties(${TARGET_NAME}
    PROPERTIES
        AUTOMOC ON
        AUTORCC ON
        FOLDER examples)

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        qskqmlexport
        qsktestsupport)