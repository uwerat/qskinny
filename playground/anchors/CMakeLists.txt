CONFIG += qskexample

list(APPEND TARGET_HEADERS
    kiwi/Constraint.h
    kiwi/Expression.h
    kiwi/Solver.h
    kiwi/Strength.h
    kiwi/Term.h
    kiwi/Variable.h

list(APPEND TARGET_SOURCES
    kiwi/Expression.cpp
    kiwi/Constraint.cpp
    kiwi/Solver.cpp

list(APPEND TARGET_HEADERS
    AnchorBox.h

list(APPEND TARGET_SOURCES
    AnchorBox.cpp
    main.cpp
