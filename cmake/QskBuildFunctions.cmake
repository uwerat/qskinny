############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

function(qsk_add_executable target)

    if(QT_VERSION_MAJOR VERSION_GREATER_EQUAL 6)
        qt6_add_executable(${ARGV})

        # we manually export our APIs to QML - might change in the future
        set_target_properties(${target} PROPERTIES 
            QT_QML_MODULE_NO_IMPORT_SCAN 1)
    else()
        add_executable(${ARGV})
    endif()

    set_target_properties(${target} PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin )

endfunction()

function(qsk_embed_sources target)

    # In cross platform scenarios you might need the qvg converter
    # tools for the build - not the target - platform. To avoid having
    # to build all libraries those tools offer a standalone
    # mode that includes some source files instead.

    # Some moc files are transitively required:
    #    - f.e #include <QskGraphic.cpp> -> #include "moc_QskGraphic.cpp"
    # Those will be generated when adding the dependency below

    add_dependencies(${target} qskinny)

    # TODO hack for standalone qvg2svg
    get_target_property(qskinny_AUTOGEN_DIR qskinny AUTOGEN_BUILD_DIR)
    if (${qskinny_AUTOGEN_DIR} STREQUAL "")
        message(FATAL_ERROR "Directory '${qskinny_AUTOGEN_DIR}' doesn't exist")
    endif()

    # TODO fix multi configuration generators
    if(CMAKE_GENERATOR MATCHES "Visual Studio.*")
        add_definitions("/I${qskinny_AUTOGEN_DIR}/include_\$(Configuration)")
    elseif(CMAKE_GENERATOR MATCHES "Ninja Multi.*")
        target_include_directories(${target}
            PRIVATE
                ${qskinny_AUTOGEN_DIR}/include_$<CONFIG>)
    else()
        target_include_directories(${target} PRIVATE ${qskinny_AUTOGEN_DIR}/include)
    endif()

    target_include_directories(${target}
        PRIVATE
            ${QSK_SOURCE_DIR}/src/common
            ${QSK_SOURCE_DIR}/src/graphic)

    target_compile_definitions(${target} PRIVATE QSK_STANDALONE)
    target_link_libraries(${target} PRIVATE Qt::Gui Qt::GuiPrivate)
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

    if(BUILD_QSKDLL)
        set(library_type SHARED)
    else()
        set(library_type STATIC)
    endif()

    if(QT_VERSION_MAJOR VERSION_GREATER_EQUAL 6)
        qt6_add_library(${target} ${library_type})
    else()
        add_library(${target} ${library_type})
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

    # pass on OUTPUT_TARGETS to the caller of this function
    cmake_parse_arguments(PARSE_ARGV 1 arg "" "OUTPUT_TARGETS" "")
    if (arg_OUTPUT_TARGETS)
        set(${arg_OUTPUT_TARGETS} ${${arg_OUTPUT_TARGETS}} PARENT_SCOPE)
    endif()

endfunction()
