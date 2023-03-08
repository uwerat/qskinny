list(APPEND TARGET_HEADERS
    ShapeItem.h
    GeometricShape.h
    Window.h
    Stroke.h)

list(APPEND TARGET_SOURCES
    Stroke.cpp
    ShapeItem.cpp
    GeometricShape.cpp
    Window.cpp
    main.cpp)

set(TARGET_NAME shapes)

qt_add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE
    ${TARGET_HEADERS}
    ${TARGET_SOURCES})

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        qskqmlexport
        qsktestsupport
        Qt::QuickShapesPrivate)

set_target_properties(${TARGET_NAME}
    PROPERTIES
        AUTOMOC ON
        FOLDER playground)
