list(APPEND TARGET_HEADERS
    Callback.h
    Invoker.h)

list(APPEND TARGET_SOURCES
    Callback.cpp
    Invoker.cpp
    main.cpp)

set(TARGET_NAME invoker)

qt_add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE
    ${TARGET_SOURCES}
    ${TARGET_HEADERS})

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny)

set_target_properties(${TARGET_NAME}
    PROPERTIES
        AUTOMOC_MOC_OPTIONS --no-warnings
        )

include(${CMAKE_SOURCE_DIR}/cmake/QskPlayground.cmake)
