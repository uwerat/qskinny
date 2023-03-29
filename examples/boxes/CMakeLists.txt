list(APPEND TARGET_HEADERS
    Box.h)

list(APPEND TARGET_SOURCES
    Box.cpp
    main.cpp)

set(TARGET_NAME boxes)

qt_add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE ${TARGET_SOURCES} ${TARGET_HEADERS})

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        qsktestsupport)

include(${CMAKE_SOURCE_DIR}/cmake/QskExamples.cmake)
