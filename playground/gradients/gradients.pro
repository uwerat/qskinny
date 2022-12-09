CONFIG += qskexample

QT += widgets quickwidgets

HEADERS += 

SOURCES += \
    main.cpp

linux {

    pedantic {
        QMAKE_CXXFLAGS += -isystem $$[QT_INSTALL_HEADERS]/QtQuickWidgets
        QMAKE_CXXFLAGS += -isystem $$[QT_INSTALL_HEADERS]/QtWidgets
    }
}
