list(APPEND TARGET_HEADERS
    ShadowedBox.h)

list(APPEND TARGET_SOURCES
    ShadowedBox.cpp
    main.cpp)

set(TARGET_NAME shadows)

qt_add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE
    ${TARGET_HEADERS}
    ${TARGET_SOURCES})

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        qsktestsupport)

set_target_properties(${TARGET_NAME}
    PROPERTIES
        AUTOMOC ON
        FOLDER playground)