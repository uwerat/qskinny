list(APPEND TARGET_HEADERS
    kiwi/Constraint.h
    kiwi/Expression.h
    kiwi/Solver.h
    kiwi/Strength.h
    kiwi/Term.h
    kiwi/Variable.h)

list(APPEND TARGET_SOURCES
    kiwi/Expression.cpp
    kiwi/Constraint.cpp
    kiwi/Solver.cpp)

list(APPEND TARGET_HEADERS
    AnchorBox.h)

list(APPEND TARGET_SOURCES
    AnchorBox.cpp
    main.cpp)

set(TARGET_NAME anchors)

add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE
    ${TARGET_SOURCES}
    ${TARGET_HEADERS}
    ${TARGET_RESSOURCES})

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        qsktestsupport)

set_target_properties(${TARGET_NAME}
    PROPERTIES
        AUTOMOC ON
        FOLDER playground)