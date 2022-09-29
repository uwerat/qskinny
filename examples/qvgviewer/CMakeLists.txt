list(APPEND TARGET_HEADERS
    MainWindow.h)

list(APPEND TARGET_SOURCES
    MainWindow.cpp
    main.cpp)

list(APPEND TARGET_RESSOURCES
    qvgviewer.qrc)

list(APPEND TARGET_SVGS
    svg/01.01.10.svg
    svg/01.03.04q.svg
    svg/01.08.05q.svg
    svg/01.25.18.svg)

list(APPEND TARGET_QVGS
    qvg/01.01.10.qvg
    qvg/01.03.04q.qvg
    qvg/01.08.05q.qvg
    qvg/01.25.18.qvg)

set(TARGET_NAME qvgviewer)

add_executable(${TARGET_NAME}
    ${TARGET_SOURCES}
    ${TARGET_HEADERS}
    ${TARGET_RESSOURCES}
    ${TARGET_SVGS})

set_target_properties(${TARGET_NAME}
    PROPERTIES
        AUTOMOC ON
        AUTORCC ON
        FOLDER examples)

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        qsktestsupport)

option(autoqvg "enable automatic qvg commpilation" OFF)

if(autoqvg)
    svg2qvg(${CMAKE_CURRENT_LIST_DIR}/svg/01.01.10.svg
            ${CMAKE_CURRENT_LIST_DIR}/qvg/01.01.10.qvg)
    svg2qvg(${CMAKE_CURRENT_LIST_DIR}/svg/01.03.04q.svg
            ${CMAKE_CURRENT_LIST_DIR}/qvg/01.03.04q.qvg)
    svg2qvg(${CMAKE_CURRENT_LIST_DIR}/svg/01.08.05q.svg
            ${CMAKE_CURRENT_LIST_DIR}/qvg/01.08.05q.qvg)
    svg2qvg(${CMAKE_CURRENT_LIST_DIR}/svg/01.25.18.svg
            ${CMAKE_CURRENT_LIST_DIR}/qvg/01.25.18.qvg)
else()
    # When cross compiling we need to have the svg2qvg tool being
    # compiled for the build environment - not for the one of the target.
    # So we better have precompiled qvg files in the repository to
    # make the build process much easier
endif()

target_sources(${TARGET_NAME} PRIVATE ${TARGET_QVGS})
source_group("generated" FILES ${TARGET_QVGS})
source_group("svg" FILES ${TARGET_SVGS})