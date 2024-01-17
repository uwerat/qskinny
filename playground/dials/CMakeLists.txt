############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES
    SkinFactory.h SkinFactory.cpp
    Dial.h Dial.cpp
    DialSkinlet.h DialSkinlet.cpp
    Dashboard.h Dashboard.cpp
    main.cpp
)
qt_add_resources(SOURCES images.qrc)

qsk_add_example(dials ${SOURCES})
