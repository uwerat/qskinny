############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES main.cpp)
set(target systemdialogs)

qsk_add_example(${target} ${SOURCES})

target_link_libraries(${target} PRIVATE
    Qt::QuickDialogs2UtilsPrivate
    Qt::QuickDialogs2Private
)
