list(APPEND TARGET_HEADERS
    CustomSlider.h
    CustomSliderSkinlet.h
    OtherSlider.h)

list(APPEND TARGET_SOURCES
    CustomSlider.cpp
    CustomSliderSkinlet.cpp
    OtherSlider.cpp
    main.cpp)

set(TARGET_NAME tabview)

qt_add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE ${TARGET_SOURCES} ${TARGET_HEADERS})

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        qsktestsupport)

include(${CMAKE_SOURCE_DIR}/cmake/QskExamples.cmake)
