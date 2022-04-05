CONFIG += qskexample

HEADERS += \
    MainWindow.h

SOURCES += \
    MainWindow.cpp \
    main.cpp

# CONFIG += autoqvg

autoqvg {

    CONFIG += qskqvg

    QVGRESOURCES += \
        qvgviewer.qrc

    SVGSOURCES = \
        svg/01.01.10.svg \
        svg/01.03.04q.svg \
        svg/01.08.05q.svg \
        svg/01.25.18.svg

} else {

    # When cross compiling we need to have the svg2qvg tool being
    # compiled for the build environment - not for the one of the target.
    # So we better have precompiled qvg files in the repository to
    # make the build process much easier

    RESOURCES += \
        qvgviewer.qrc
}
