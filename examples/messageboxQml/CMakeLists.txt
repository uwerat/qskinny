############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
# This file may be used under the terms of the 3-clause BSD License
############################################################################

set(SOURCES main.cpp)
qt_add_resources(SOURCES messagebox.qrc)

qsk_add_example(messageboxQml ${SOURCES})
