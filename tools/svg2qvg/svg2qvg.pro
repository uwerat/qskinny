TEMPLATE     = app
TARGET = svg2qvg

QT += svg

CONFIG += standalone
CONFIG -= app_bundle
CONFIG -= sanitize

DESTDIR      = $${QSK_OUT_ROOT}/tools/bin

standalone {

    # We only need a very small subset of QSkinny and by including the 
    # necessary cpp files svg2qvg becomes independent from the library

    QSK_DIRS = \
        $${QSK_ROOT}/src/common \
        $${QSK_ROOT}/src/graphic

    INCLUDEPATH *= $${QSK_DIRS}
    DEPENDPATH  += $${QSK_DIRS}

    DEFINES += QSK_STANDALONE
    QSK_CONFIG -= QskDll

    QT += gui-private
}
else {

    CONFIG += qskinny
}

SOURCES += \
    main.cpp

target.path    = $${QSK_INSTALL_BINS}
INSTALLS       = target
