
list(APPEND TARGET_HEADERS
    GridAccessor.h
    GridSkinny.h
    GridWidgets.h
    GridGraphics.h
    GridQuick.h
    TestBox.h)

list(APPEND TARGET_SOURCES
    GridAccessor.cpp
    GridSkinny.cpp
    GridWidgets.cpp
    GridGraphics.cpp
    GridQuick.cpp
    TestBox.cpp
    main.cpp)

set(TARGET_NAME grid)

add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE
    ${TARGET_SOURCES}
    ${TARGET_HEADERS})

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        qsktestsupport
        ${Qt}::QuickWidgets)

set_target_properties(${TARGET_NAME}
    PROPERTIES
        AUTOMOC ON
        FOLDER playground)