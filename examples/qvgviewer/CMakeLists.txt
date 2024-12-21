############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES
    MainWindow.h MainWindow.cpp
    SymbolProvider.h SymbolProvider.cpp
    QskSymbolProvider.h QskSymbolProvider.cpp
    main.cpp
)

qsk_add_example(qvgviewer ${SOURCES})
