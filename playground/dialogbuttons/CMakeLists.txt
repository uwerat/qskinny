set(TARGET_NAME qskexample)

list(APPEND TARGET_HEADERS
    Window.h)

list(APPEND TARGET_SOURCES
    Window.cpp
    main.cpp)

add_executable(${TARGET_NAME} ${TARGET_SOURCES} ${TARGET_HEADERS})

target_link_libraries(${TARGET_NAME}
   PRIVATE
       qskinny
       qsktestsupport)

set_target_properties(${TARGET_NAME}
    PROPERTIES
        AUTOMOC ON
        FOLDER examples)