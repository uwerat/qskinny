############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES MainWindow.h MainWindow.cpp main.cpp)
qt_add_resources(SOURCES qvgviewer.qrc)

qsk_add_example(qvgviewer ${SOURCES})
