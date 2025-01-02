############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES
    GlyphBrowser.h GlyphBrowser.cpp
    GlyphTable.h GlyphTable.cpp
    GraphicProvider.h GraphicProvider.cpp
    main.cpp
)

qt_add_resources(SOURCES
    fonts.qrc
)

qsk_add_example(glyphbrowser ${SOURCES})
