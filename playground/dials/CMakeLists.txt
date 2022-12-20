list(APPEND TARGET_HEADERS
    SkinFactory.h
    Dial.h
    DialSkinlet.h
    Dashboard.h)

list(APPEND TARGET_SOURCES
    SkinFactory.cpp
    Dial.cpp
    DialSkinlet.cpp
    Dashboard.cpp
    main.cpp)

list(APPEND TARGET_RESSOURCES
    images.qrc)

set(TARGET_NAME dials)

qt_add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE
    ${TARGET_SOURCES}
    ${TARGET_HEADERS}
    ${TARGET_RESSOURCES})

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        qsktestsupport)

set_target_properties(${TARGET_NAME}
    PROPERTIES
        AUTOMOC ON
        FOLDER playground)