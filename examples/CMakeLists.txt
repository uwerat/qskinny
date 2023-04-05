############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
# This file may be used under the terms of the 3-clause BSD License
############################################################################

add_subdirectory(desktop)
add_subdirectory(gallery)
add_subdirectory(layouts)
add_subdirectory(listbox)
add_subdirectory(messagebox)
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
    add_subdirectory(messageboxQml)
endif()
