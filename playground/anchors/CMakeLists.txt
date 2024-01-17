############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES
    kiwi/Strength.h kiwi/Term.h kiwi/Variable.h
    kiwi/Constraint.h kiwi/Constraint.cpp
    kiwi/Expression.h kiwi/Expression.cpp
    kiwi/Solver.h kiwi/Solver.cpp
    AnchorBox.h AnchorBox.cpp
    main.cpp
)

qsk_add_example(anchors ${SOURCES})
