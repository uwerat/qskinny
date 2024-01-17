if(TARGET Qt::Svg)
    add_subdirectory(svg2qvg)
    install(
        FILES
            ${QSK_CMAKE_DIR}/QSkinnyTools.cmake
        DESTINATION
            ${PACKAGE_LOCATION}
        COMPONENT
            Devel)
endif()
