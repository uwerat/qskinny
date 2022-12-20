
list(APPEND TARGET_SOURCES
    main.cpp)

set(TARGET_NAME webview)

qt_add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE
    ${TARGET_SOURCES})

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        qsktestsupport)

if (QT_VERSION_MAJOR VERSION_GREATER "5")
    target_link_libraries(${TARGET_NAME}
        PRIVATE
            Qt::WebEngineQuick
            Qt::WebEngineQuickPrivate)
else()
    target_link_libraries(${TARGET_NAME}
        PRIVATE
            Qt::WebEngine
            Qt::WebEnginePrivate)
endif()

set_target_properties(${TARGET_NAME}
    PROPERTIES
        AUTOMOC ON
        FOLDER playground)