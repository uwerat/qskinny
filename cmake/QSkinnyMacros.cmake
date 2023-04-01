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

function(qsk_add_plugin PLUGIN_NAME TYPE CLASS_NAME)

    # qt6_add_plugin calls add_library as MODULE - even when SHARED
    # is requested. In general this ould be the correct type for plugins -
    # however we also want to link the plugin when ENABLE_ENSURE_SKINS
    # is enabled.
    # Probably the correct solution would be to put the implemetation of
    # the skins into a regular lib and the plugins would contain
    # the factories only. TODO ...
    # So for the moment better don't do:
    #      qt6_add_plugin(${PLUGIN_NAME} SHARED ${CLASS_NAME} )

    if(QT_VERSION_MAJOR VERSION_GREATER_EQUAL 6)
        qt6_add_library(${PLUGIN_NAME} SHARED )
    else()
        add_library(${PLUGIN_NAME} SHARED )
    endif()

    set_target_properties(${PLUGIN_NAME} PROPERTIES
        QT_PLUGIN_CLASS_NAME ${CLASS_NAME} )

    target_compile_definitions(${PLUGIN_NAME} PRIVATE QT_PLUGIN )

    target_sources( ${PLUGIN_NAME} PRIVATE ${HEADERS} ${SOURCES} ${RESOURCES} )
    target_link_libraries(${PLUGIN_NAME} PRIVATE qskinny)

    set_target_properties(${PLUGIN_NAME} PROPERTIES FOLDER ${TYPE})

    # the plugin has to go to .../plugins/${TYPE}/
    # otherwise it won't be found at runtime ...

    set_target_properties( ${PLUGIN_NAME} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/plugins/${TYPE}")

endfunction()

function(qsk_add_example EXAMPLE_NAME)

    set(TARGET_NAME ${EXAMPLE_NAME})

    qsk_add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE
        ${SOURCES} ${HEADERS} ${RESOURCES})

    set_target_properties(${TARGET_NAME} PROPERTIES FOLDER examples)

    set_target_properties(${TARGET_NAME} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../bin )

    target_link_libraries(${TARGET_NAME} PRIVATE qskinny )

    # not all examples need this one. TODO ...
    target_link_libraries(${TARGET_NAME} PRIVATE qsktestsupport)

    if( BUILD_QML_EXPORT )
        # Only needed, when having QML files. Autodetections is possible
        # by looking into all '*.qrc' files ( rcc --list *.qrc ) for *.qml
        # entries TODO ...

        target_link_libraries(${TARGET_NAME} PRIVATE qskqmlexport)
    endif()

    # for examples with subdirectories 
    target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})

endfunction()
