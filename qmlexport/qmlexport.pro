TEMPLATE = lib
TARGET   = $$qskLibraryTarget(qskqmlexport)

QT += quick-private
CONFIG += qskinny

contains(QSK_CONFIG, QskDll): DEFINES += QSK_QML_MAKEDLL

HEADERS += \
    QskQmlGlobal.h \
    QskShortcutQml.h \
    QskLayoutQml.h \
    QskRgbValueQml.h \
    QskMainQml.h \
    QskQml.h

SOURCES += \
    QskShortcutQml.cpp \
    QskLayoutQml.cpp \
    QskMainQml.cpp \
    QskQml.cpp

target.path = $${QSK_INSTALL_LIBS}
INSTALLS    = target
