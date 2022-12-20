list(APPEND TARGET_RESSOURCES
    images.qrc)

list(APPEND TARGET_HEADERS
    Image.h)

list(APPEND TARGET_SOURCES
    Image.cpp
    main.cpp)

set(TARGET_NAME images)

qt_add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE
    ${TARGET_SOURCES}
    ${TARGET_HEADERS}
    ${TARGET_RESSOURCES})

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        qskqmlexport
        qsktestsupport)

set_target_properties(${TARGET_NAME}
    PROPERTIES
        AUTOMOC ON
        FOLDER playground)