CONFIG += qskexample
QT += widgets quickwidgets

HEADERS += \
    GridAccessor.h \
    GridSkinny.h \
    GridWidgets.h \
    GridGraphics.h \
    GridQuick.h \
    TestBox.h

SOURCES += \
    GridAccessor.cpp \
    GridSkinny.cpp \
    GridWidgets.cpp \
    GridGraphics.cpp \
    GridQuick.cpp \
    TestBox.cpp \
    main.cpp

linux {

    pedantic {
        QMAKE_CXXFLAGS += -isystem $$[QT_INSTALL_HEADERS]/QtQuickWidgets
        QMAKE_CXXFLAGS += -isystem $$[QT_INSTALL_HEADERS]/QtWidgets
    }
}
