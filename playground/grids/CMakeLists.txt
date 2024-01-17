############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES 
    GridAccessor.h GridAccessor.cpp
    GridSkinny.h GridSkinny.cpp
    GridWidgets.h GridWidgets.cpp
    GridGraphics.h GridGraphics.cpp
    GridQuick.h GridQuick.cpp
    TestBox.h TestBox.cpp
    main.cpp
)

qsk_add_example(grids ${SOURCES})
target_link_libraries(grids PRIVATE Qt::QuickWidgets)

