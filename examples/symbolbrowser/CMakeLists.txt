############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(CMAKE_CXX_FLAGS "-Wsuggest-override -Werror=suggest-override")

set(SOURCES
    MainWindow.h MainWindow.cpp
    GlyphTable.h GlyphTable.cpp
    SymbolProvider.h SymbolProvider.cpp
    main.cpp
)

qt_add_resources(SOURCES
    fonts.qrc
)

qsk_add_example(symbolbrowser ${SOURCES})
