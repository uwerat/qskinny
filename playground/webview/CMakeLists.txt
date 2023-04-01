############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
# This file may be used under the terms of the 3-clause BSD License
############################################################################

qsk_add_example(webview main.cpp)

if (QT_VERSION_MAJOR VERSION_GREATER "5")
    target_link_libraries(webview PRIVATE Qt::WebEngineQuick Qt::WebEngineQuickPrivate)
else()
    target_link_libraries(webview PRIVATE Qt::WebEngine Qt::WebEnginePrivate)
endif()

