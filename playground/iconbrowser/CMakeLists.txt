############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES
    GlyphListView.h GlyphListView.cpp
    main.cpp
)

qt_add_resources(SOURCES
    fonts.qrc
)

qsk_add_example(iconbrowser ${SOURCES})
