############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

add_subdirectory(desktop)
add_subdirectory(gallery)
add_subdirectory(layouts)
add_subdirectory(mycontrols)
add_subdirectory(qvgviewer)
add_subdirectory(thumbnails)
add_subdirectory(tabview)
add_subdirectory(iotdashboard)

if( BUILD_QML_EXPORT )
    add_subdirectory(boxes)
    add_subdirectory(buttons)
    add_subdirectory(frames)
    add_subdirectory(glabels)
endif()
