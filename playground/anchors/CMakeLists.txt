############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
# This file may be used under the terms of the 3-clause BSD License
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
