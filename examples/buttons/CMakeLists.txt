############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES main.cpp)
qt_add_resources(SOURCES buttons.qrc)

qsk_add_example(buttons ${SOURCES})
