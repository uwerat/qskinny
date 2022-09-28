CONFIG += qskexample
target_link_libraries(${TARGET_NAME} PRIVATE widgets quickwidgets)

list(APPEND TARGET_HEADERS
    GridAccessor.h
    GridSkinny.h
    GridWidgets.h
    GridGraphics.h
    GridQuick.h
    TestBox.h

list(APPEND TARGET_SOURCES
    GridAccessor.cpp
    GridSkinny.cpp
    GridWidgets.cpp
    GridGraphics.cpp
    GridQuick.cpp
    TestBox.cpp
    main.cpp

linux {
    pedantic
    {
        QMAKE_CXXFLAGS += -isystem $$[ QT_INSTALL_HEADERS ] / QtQuickWidgets QMAKE_CXXFLAGS +=
            -isystem $$[ QT_INSTALL_HEADERS ] / QtWidgets
   }
}
