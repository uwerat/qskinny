list(APPEND TARGET_RESSOURCES
    buttons.qrc)

list(APPEND TARGET_SOURCES
    main.cpp)

set(TARGET_NAME buttons)

add_executable(${TARGET_NAME} ${TARGET_SOURCES} ${TARGET_RESSOURCES})

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        qsktestsupport
        qskqmlexport)

set_target_properties(${TARGET_NAME}
    PROPERTIES
        AUTOMOC ON
        AUTORCC ON
        FOLDER examples)