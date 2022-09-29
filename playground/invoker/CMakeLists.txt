list(APPEND TARGET_HEADERS
    Callback.h
    Invoker.h)

list(APPEND TARGET_SOURCES
    Callback.cpp
    Invoker.cpp
    main.cpp)

set(TARGET_NAME invoker)

add_executable(${TARGET_NAME}
    ${TARGET_SOURCES}
    ${TARGET_HEADERS})

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny)

set_target_properties(${TARGET_NAME}
    PROPERTIES
        AUTOMOC ON
        AUTOMOC_MOC_OPTIONS -nw
        FOLDER playground)