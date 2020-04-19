CONFIG += qskexample

HEADERS += \
    kiwi/Constraint.h \
    kiwi/Expression.h \
    kiwi/Solver.h \
    kiwi/Strength.h \
    kiwi/Term.h \
    kiwi/Variable.h

SOURCES += \
    kiwi/Expression.cpp \
    kiwi/Constraint.cpp \
    kiwi/Solver.cpp

HEADERS += \
    AnchorBox.h

SOURCES += \
    AnchorBox.cpp \
    main.cpp
