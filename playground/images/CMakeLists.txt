############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES Image.h Image.cpp main.cpp)
qt_add_resources(SOURCES images.qrc)

qsk_add_example(images ${SOURCES})
