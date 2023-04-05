############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
# This file may be used under the terms of the 3-clause BSD License
############################################################################

set(SOURCES Image.h Image.cpp main.cpp)
qt_add_resources(SOURCES images.qrc)

qsk_add_example(images ${SOURCES})
