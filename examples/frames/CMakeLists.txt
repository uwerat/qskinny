############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES Frame.h Frame.cpp main.cpp)
qt_add_resources(SOURCES frames.qrc)

qsk_add_example(frames ${SOURCES})

