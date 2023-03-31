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

function(qsk_add_executable)

    if(QT_VERSION_MAJOR VERSION_GREATER_EQUAL 6)
        qt6_add_executable(${ARGV})
    else()
        add_executable(${ARGV})
    endif()

endfunction()

function(qsk_add_library)

    if(QT_VERSION_MAJOR VERSION_GREATER_EQUAL 6)
        qt6_add_library(${ARGV})
    else()
        add_library(${ARGV})
    endif()

    set_target_properties(${TARGET_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib" )

endfunction()

function(qsk_plugin PLUGIN_NAME TYPE)

    if(QT_VERSION_MAJOR VERSION_GREATER_EQUAL 6)
        qt6_add_library(${PLUGIN_NAME} SHARED ${HEADERS} ${SOURCES} ${OTHER_FILES})
    else()
        add_library(${PLUGIN_NAME} SHARED ${HEADERS} ${SOURCES} ${OTHER_FILES})
    endif()

    target_link_libraries(${PLUGIN_NAME} PRIVATE qskinny)

    set_target_properties(${PLUGIN_NAME} PROPERTIES FOLDER ${TYPE})

    # the plugin has to go to .../plugins/${TYPE}/
    # otherwise it can't be loaded at runtime ...

    set_target_properties( ${PLUGIN_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins/${TYPE}")

endfunction()

function(qsk_example EXAMPLE_NAME)

    set(TARGET_NAME ${EXAMPLE_NAME})

    qsk_add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE
        ${SOURCES} ${HEADERS} ${RESOURCES})

    set_target_properties(${TARGET_NAME} PROPERTIES FOLDER examples)

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
