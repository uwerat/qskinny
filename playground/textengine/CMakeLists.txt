############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(target textengine)

set(SOURCES
    TextView.h
    TextView.cpp
    main.cpp
)

qsk_add_example(${target} ${SOURCES})
