list(APPEND TARGET_SOURCES
    main.cpp)

set(TARGET_NAME desktop)

qt_add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE ${TARGET_SOURCES})

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        qsktestsupport)

include(${CMAKE_SOURCE_DIR}/cmake/QskExamples.cmake)
