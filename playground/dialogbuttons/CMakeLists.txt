set(TARGET_NAME qskexample)

list(APPEND TARGET_HEADERS
    Window.h)

list(APPEND TARGET_SOURCES
    Window.cpp
    main.cpp)

qt_add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE ${TARGET_SOURCES} ${TARGET_HEADERS})

target_link_libraries(${TARGET_NAME}
   PRIVATE
       qskinny
       qsktestsupport)

include(${CMAKE_SOURCE_DIR}/cmake/QskPlayground.cmake)
