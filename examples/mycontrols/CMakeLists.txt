list(APPEND TARGET_SOURCES
    MySkin.cpp
    MyToggleButton.cpp
    MyToggleButtonSkinlet.cpp
    main.cpp)

list(APPEND TARGET_HEADERS
    MyToggleButton.h
    MyToggleButtonSkinlet.h
    MySkin.h)

set(TARGET_NAME mycontrols)

add_executable(${TARGET_NAME} ${TARGET_SOURCES} ${TARGET_HEADERS})

set_target_properties(${TARGET_NAME}
    PROPERTIES
        AUTOMOC ON
        AUTORCC ON
        FOLDER examples)

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        qsktestsupport)