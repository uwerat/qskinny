############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

function(qsk_add_executable target)

    if(QT_VERSION_MAJOR VERSION_GREATER_EQUAL 6)
        qt6_add_executable(${ARGV})
    else()
        add_executable(${ARGV})
    endif()

    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

endfunction()

function(qsk_add_library target)

    if(QT_VERSION_MAJOR VERSION_GREATER_EQUAL 6)
        qt6_add_library(${ARGV})
    else()
        add_library(${ARGV})
    endif()

    set_target_properties(${target} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib )

endfunction()

function(qsk_add_plugin target TYPE CLASS_NAME)

    cmake_parse_arguments(PARSE_ARGV 3 arg "MANUAL_FINALIZATION" "" "")

    # qt6_add_plugin calls add_library as MODULE - even when SHARED
    # is requested. In general this ould be the correct type for plugins -
    # however we also want to link the plugin when ENABLE_ENSURE_SKINS
    # is enabled.
    # Probably the correct solution would be to put the implemetation of
    # the skins into a regular lib and the plugins would contain
    # the factories only. TODO ...
    # So for the moment better don't do:
    #      qt6_add_plugin(${target} SHARED ${CLASS_NAME} )

    if(QT_VERSION_MAJOR VERSION_GREATER_EQUAL 6)
        qt6_add_library(${target} SHARED )
    else()
        add_library(${target} SHARED )
    endif()

    set_target_properties(${target} PROPERTIES
        QT_PLUGIN_CLASS_NAME ${CLASS_NAME} )

    target_compile_definitions(${target} PRIVATE QT_PLUGIN )

    target_sources( ${target} PRIVATE ${arg_UNPARSED_ARGUMENTS} )
    target_link_libraries(${target} PRIVATE qskinny)

    set_target_properties(${target} PROPERTIES FOLDER ${TYPE})

    # the plugin has to go to .../plugins/${TYPE}/
    # otherwise it won't be found at runtime ...

    set_target_properties( ${target} PROPERTIES
        LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/plugins/${TYPE})

    install(TARGETS ${target} DESTINATION "${QSK_INSTALL_LIBS}/plugins/${TYPE}" )
    set_target_properties(${target} PROPERTIES
        INSTALL_RPATH "\${ORIGIN}/../../lib" )

endfunction()

function(qsk_add_example target)

    cmake_parse_arguments(PARSE_ARGV 1 arg "MANUAL_FINALIZATION" "" "")

    qsk_add_executable(${target} WIN32 MACOSX_BUNDLE ${arg_UNPARSED_ARGUMENTS} )

    set_target_properties(${target} PROPERTIES FOLDER examples)

    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/../bin )

    target_link_libraries(${target} PRIVATE qskinny )

    # not all examples need this one. TODO ...
    target_link_libraries(${target} PRIVATE qsktestsupport)

    if( BUILD_QML_EXPORT )
        # Only needed, when having QML files. Autodetections is possible
        # by looking into all '*.qrc' files ( rcc --list *.qrc ) for *.qml
        # entries TODO ...

        target_link_libraries(${target} PRIVATE qskqmlexport)
    endif()

    # for examples with subdirectories 
    target_include_directories(${target} PRIVATE ${CMAKE_CURRENT_LIST_DIR})

endfunction()

function(qsk_add_shaders target)

    cmake_parse_arguments( arg "" "" "FILES" ${ARGN} )

    # assuming that OUTPUTS is not set in ARGV
    foreach( file IN LISTS arg_FILES )
        get_filename_component(qsbname "${file}" NAME)
        string(REPLACE "-vulkan" "" qsbname "${qsbname}" )
        list(APPEND outfiles "${qsbname}.qsb")
    endforeach()

    qt6_add_shaders( ${target} "qskshaders" BATCHABLE PRECOMPILE QUIET
        PREFIX "/qskinny/shaders" ${ARGV} OUTPUTS ${outfiles} )

endfunction()
