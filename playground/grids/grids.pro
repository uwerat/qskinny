CONFIG += qskexample
QT += widgets quickwidgets

HEADERS += \
    GridAccessor.h \
    GridSkinny.h \
    GridWidgets.h \
    GridGraphics.h \
    TestBox.h

SOURCES += \
    GridAccessor.cpp \
    GridSkinny.cpp \
    GridWidgets.cpp \
    GridGraphics.cpp \
    TestBox.cpp \
    main.cpp

linux {

    pedantic {
        QMAKE_CXXFLAGS += -isystem $$[QT_INSTALL_HEADERS]/QtQuickWidgets
        QMAKE_CXXFLAGS += -isystem $$[QT_INSTALL_HEADERS]/QtWidgets
    }
}
