include( $${PWD}/../examples.pri )

TARGET = symbols

QT += quick quick-private
CONFIG += no_private_qt_headers_warning

HEADERS += \
    ButtonBox.h

SOURCES += \
    ButtonBox.cpp \
    main.cpp
