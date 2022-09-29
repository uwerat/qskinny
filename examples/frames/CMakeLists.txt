list(APPEND TARGET_RESSOURCES
    frames.qrc)

list(APPEND TARGET_HEADERS
    Frame.h)

list(APPEND TARGET_SOURCES
    Frame.cpp
    main.cpp)

set(TARGET_NAME frames)

add_executable(${TARGET_NAME} ${TARGET_SOURCES} ${TARGET_HEADERS} ${TARGET_RESSOURCES})

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