############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(target svg2qvg)
qsk_add_executable(${target} main.cpp)

if(BUILD_SVG2QVG_STANDALONE)

    # NOTE: when building standalone some moc files are transitively required.
    #       These files are being created by the qskinny build, hence we add an explicit dependency.
    #       E.g. main.cpp -> #include <QskGraphic.cpp> -> #include "moc_QskGraphic.cpp"
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
else()
    target_link_libraries(${target} PRIVATE qskinny)
endif()

target_link_libraries(${target} PRIVATE Qt::Svg)

set_target_properties(${target} PROPERTIES FOLDER tools)

install(TARGETS ${target})

# packaging
set(PACKAGE_TARGET_FILENAME QSkinnyToolsTargets.cmake)

set_target_properties(${target} PROPERTIES EXPORT_NAME Svg2Qvg)
set_target_properties(${target} PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)

install(TARGETS ${target} EXPORT ${PACKAGE_TARGET_FILENAME})

export(EXPORT ${PACKAGE_TARGET_FILENAME}
    NAMESPACE ${PACKAGE_NAMESPACE})

install(EXPORT ${PACKAGE_TARGET_FILENAME}
    FILE
        ${PACKAGE_TARGET_FILENAME}
    NAMESPACE
        ${PACKAGE_NAMESPACE}
    DESTINATION
        ${PACKAGE_LOCATION})

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    install(FILES $<TARGET_PDB_FILE:${target}> 
        DESTINATION
            ${CMAKE_INSTALL_BINDIR} OPTIONAL)
endif()