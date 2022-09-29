list(APPEND TARGET_HEADERS
    MainWindow.h)

list(APPEND TARGET_SOURCES
    MainWindow.cpp
    main.cpp)

set(TARGET_NAME qvgviewer)

#CONFIG += autoqvg

option(autoqvg "enable automatic qvg commpilation" OFF)

if(autoqvg)
    # CONFIG += qskqvg # TODO why?

    list(APPEND TARGET_RESSOURCES
        qvgviewer.qrc)

    list(APPEND SVGSOURCES
        svg/01.01.10.svg
        svg/01.03.04q.svg
        svg/01.08.05q.svg
        svg/01.25.18.svg)
else()
    # When cross compiling we need to have the svg2qvg tool being
    # compiled for the build environment - not for the one of the target.
    # So we better have precompiled qvg files in the repository to
    # make the build process much easier
    list(APPEND TARGET_RESSOURCES
        qvgviewer.qrc)
endif()

add_executable(${TARGET_NAME} ${TARGET_SOURCES} ${TARGET_HEADERS} ${TARGET_RESSOURCES})

set_target_properties(${TARGET_NAME}
    PROPERTIES
        AUTOMOC ON
        AUTORCC ON
        FOLDER examples)

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        qsktestsupport)