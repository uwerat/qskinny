list(APPEND TARGET_RESSOURCES
    messagebox.qrc)

list(APPEND TARGET_SOURCES
    main.cpp)

set(TARGET_NAME messageboxQml)

qt_add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE ${TARGET_SOURCES} ${TARGET_RESSOURCES})

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        qsktestsupport
        qskqmlexport)

set_target_properties(${TARGET_NAME}
    PROPERTIES
        AUTOMOC ON
        AUTORCC ON
        FOLDER examples)