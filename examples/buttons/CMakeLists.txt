list(APPEND TARGET_RESSOURCES
    buttons.qrc)

list(APPEND TARGET_SOURCES
    main.cpp)

set(TARGET_NAME buttons)

qt_add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE ${TARGET_SOURCES} ${TARGET_RESSOURCES})

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        qsktestsupport
        qskqmlexport)

include(${CMAKE_SOURCE_DIR}/cmake/QskExamples.cmake)
