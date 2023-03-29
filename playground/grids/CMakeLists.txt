list(APPEND HEADERS
    GridAccessor.h
    GridSkinny.h
    GridWidgets.h
    GridGraphics.h
    GridQuick.h
    TestBox.h)

list(APPEND SOURCES
    GridAccessor.cpp
    GridSkinny.cpp
    GridWidgets.cpp
    GridGraphics.cpp
    GridQuick.cpp
    TestBox.cpp
    main.cpp)

qsk_example(grids)
target_link_libraries(grids PRIVATE Qt::QuickWidgets)

