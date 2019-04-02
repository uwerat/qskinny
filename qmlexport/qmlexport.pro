TEMPLATE = lib
TARGET   = $$qskLibraryTarget(qskqmlexport)

QT += quick-private
CONFIG += qskinny

contains(QSK_CONFIG, QskDll): DEFINES += QSK_QML_MAKEDLL

HEADERS += \
    QskQmlGlobal.h \
    QskShortcut.h \
    QskQml.h

SOURCES += \
    QskShortcut.cpp \
    QskQml.cpp

target.path = $${QSK_INSTALL_LIBS}
INSTALLS    = target
