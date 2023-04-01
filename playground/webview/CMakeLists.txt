list(APPEND SOURCES main.cpp)

qsk_add_example(webview)

if (QT_VERSION_MAJOR VERSION_GREATER "5")
    target_link_libraries(webview PRIVATE Qt::WebEngineQuick Qt::WebEngineQuickPrivate)
else()
    target_link_libraries(webview PRIVATE Qt::WebEngine Qt::WebEnginePrivate)
endif()

