list(APPEND TARGET_SOURCES
    main.cpp)

set(TARGET_NAME inputpanel)

add_executable(${TARGET_NAME}
    ${TARGET_SOURCES})

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        qsktestsupport)

set_target_properties(${TARGET_NAME}
    PROPERTIES
        AUTOMOC ON
        FOLDER playground)
