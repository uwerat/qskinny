CONFIG += qskexample

list(APPEND TARGET_HEADERS
    MainWindow.h

list(APPEND TARGET_SOURCES
    MainWindow.cpp
    main.cpp

#CONFIG += autoqvg

autoqvg {
    CONFIG += qskqvg

    QVGlist(APPEND TARGET_RESSOURCES
        qvgviewer.qrc

    SVGSOURCES =
        svg/01.01.10.svg
        svg/01.03.04q.svg
        svg/01.08.05q.svg
        svg/01.25.18.svg

} else {

#When cross compiling we need to have the svg2qvg tool being
#compiled for the build environment - not for the one of the target.
#So we better have precompiled qvg files in the repository to
#make the build process much easier
    list(APPEND TARGET_RESSOURCES
        qvgviewer.qrc
}
