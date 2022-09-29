
list(APPEND TARGET_HEADERS
    GridAccessor.h
    GridSkinny.h
    GridWidgets.h
    GridGraphics.h
    GridQuick.h
    TestBox.h)

list(APPEND TARGET_SOURCES
    GridAccessor.cpp
    GridSkinny.cpp
    GridWidgets.cpp
    GridGraphics.cpp
    GridQuick.cpp
    TestBox.cpp
    main.cpp)

set(TARGET_NAME grid)

add_executable(${TARGET_NAME}
    ${TARGET_SOURCES}
    ${TARGET_HEADERS})

# TODO
#linux {
#    pedantic
#    {
#        QMAKE_CXXFLAGS += -isystem $$[ QT_INSTALL_HEADERS ] / QtQuickWidgets QMAKE_CXXFLAGS +=
#            -isystem $$[ QT_INSTALL_HEADERS ] / QtWidgets
#   }
#}

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        qsktestsupport
        Qt5::QuickWidgets)

set_target_properties(${TARGET_NAME}
    PROPERTIES
        AUTOMOC ON
        FOLDER playground)