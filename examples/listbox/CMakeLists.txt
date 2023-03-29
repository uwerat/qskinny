list(APPEND TARGET_SOURCES
    main.cpp)

set(TARGET_NAME listbox)

qt_add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE ${TARGET_SOURCES} ${TARGET_HEADERS} ${TARGET_RESSOURCES})

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        qsktestsupport)

include(${CMAKE_SOURCE_DIR}/cmake/QskExamples.cmake)
