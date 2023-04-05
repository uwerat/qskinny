############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
# This file may be used under the terms of the 3-clause BSD License
############################################################################

set(SOURCES main.cpp)
qt_add_resources(SOURCES glabels.qrc)

qsk_add_example(glabels ${SOURCES})
