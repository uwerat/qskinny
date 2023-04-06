############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

qsk_add_example(webview main.cpp)

if (QT_VERSION_MAJOR VERSION_GREATER 5)
    target_link_libraries(webview PRIVATE Qt::WebEngineQuick Qt::WebEngineQuickPrivate)
else()
    target_link_libraries(webview PRIVATE Qt::WebEngine Qt::WebEnginePrivate)
endif()

