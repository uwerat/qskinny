list(APPEND TARGET_SOURCES
    main.cpp)

set(TARGET_NAME messagebox)

add_executable(${TARGET_NAME} ${TARGET_SOURCES})

set_target_properties(${TARGET_NAME}
    PROPERTIES
        AUTOMOC ON
        AUTORCC ON
        FOLDER examples)

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        qsktestsupport)