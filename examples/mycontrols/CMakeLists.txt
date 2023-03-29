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

qt_add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE ${TARGET_SOURCES} ${TARGET_HEADERS})

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        qsktestsupport)

include(${CMAKE_SOURCE_DIR}/cmake/QskExamples.cmake)
