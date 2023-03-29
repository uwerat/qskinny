if(TARGET ${Qt}::Svg)
    ## @param SVG_FILENAME absolute filename to the svg
    ## @param QVG_FILENAME absolute filename to the qvg
    function(qsk_svg2qvg SVG_FILENAME QVG_FILENAME)
        get_filename_component(QVG_FILENAME ${QVG_FILENAME} ABSOLUTE)
        get_filename_component(SVG_FILENAME ${SVG_FILENAME} ABSOLUTE)
        add_custom_command(
            COMMAND svg2qvg ${SVG_FILENAME} ${QVG_FILENAME}
            OUTPUT ${QVG_FILENAME}
            DEPENDS ${SVG_FILENAME}
            WORKING_DIRECTORY $<TARGET_FILE_DIR:${Qt}::Svg>
            COMMENT "Compiling ${SVG_FILENAME} to ${QVG_FILENAME}")
    endfunction()
endif()

function(qsk_example EXAMPLE_NAME)

    set(TARGET_NAME ${EXAMPLE_NAME})

    qt_add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE
        ${SOURCES} ${HEADERS} ${RESOURCES})

    set_target_properties(${TARGET_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../bin )

    target_link_libraries(${TARGET_NAME} PRIVATE qskinny )

    # not all examples need this one. TODO ...
    target_link_libraries(${TARGET_NAME} PRIVATE qsktestsupport)

    # only needed, when having QML files, autodetection might be possible
    target_link_libraries(${TARGET_NAME} PRIVATE qskqmlexport)

    # for examples with subdirectories 
    target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})

endfunction()
