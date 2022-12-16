list(APPEND TARGET_HEADERS
    Box.h)

list(APPEND TARGET_SOURCES
    Box.cpp
    main.cpp)

set(TARGET_NAME boxes)

add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE ${TARGET_SOURCES} ${TARGET_HEADERS})

set_target_properties(${TARGET_NAME}
    PROPERTIES
        AUTOMOC ON
        FOLDER examples)

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        qsktestsupport)