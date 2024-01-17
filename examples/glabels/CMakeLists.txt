############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES main.cpp)
qt_add_resources(SOURCES glabels.qrc)

qsk_add_example(glabels ${SOURCES})
