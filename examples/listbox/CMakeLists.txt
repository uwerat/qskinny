list(APPEND TARGET_SOURCES
    main.cpp)

set(TARGET_NAME listbox)

qt_add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE ${TARGET_SOURCES} ${TARGET_HEADERS} ${TARGET_RESSOURCES})

set_target_properties(${TARGET_NAME}
    PROPERTIES
        AUTOMOC ON
        AUTORCC ON
        FOLDER examples)

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        qsktestsupport)