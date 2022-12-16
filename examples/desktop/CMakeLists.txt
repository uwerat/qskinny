list(APPEND TARGET_SOURCES
    main.cpp)

set(TARGET_NAME desktop)

add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE ${TARGET_SOURCES})

set_target_properties(${TARGET_NAME}
    PROPERTIES
        AUTOMOC ON
        FOLDER examples)

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        qsktestsupport)