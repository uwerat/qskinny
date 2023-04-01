############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
# This file may be used under the terms of the 3-clause BSD License
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

