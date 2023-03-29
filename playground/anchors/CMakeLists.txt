list(APPEND HEADERS
    kiwi/Constraint.h
    kiwi/Expression.h
    kiwi/Solver.h
    kiwi/Strength.h
    kiwi/Term.h
    kiwi/Variable.h)

list(APPEND SOURCES
    kiwi/Expression.cpp
    kiwi/Constraint.cpp
    kiwi/Solver.cpp)

list(APPEND HEADERS
    AnchorBox.h)

list(APPEND SOURCES
    AnchorBox.cpp
    main.cpp)

qsk_example(anchors)
