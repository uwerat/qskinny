############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES
    GlyphListView.h GlyphListView.cpp
    GlyphTable.h GlyphTable.cpp
    main.cpp
)

qt_add_resources(SOURCES
    fonts.qrc
)

qsk_add_example(glyphbrowser ${SOURCES})
