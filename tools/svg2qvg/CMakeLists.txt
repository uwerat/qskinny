set(TARGET_NAME svg2qvg)
set(TARGET_INSTALL_SUFFIX bin/tools)
set(TARGET_SOURCES main.cpp)

option(BUILD_SVG2QVG_STANDALONE "If 'ON' application won't be linked against qskinny library" ON)

add_executable(${TARGET_NAME} ${TARGET_SOURCES})

if(BUILD_SVG2QVG_STANDALONE)

    # NOTE: when building standalone some moc files are transitively required.
    #       These files are being created by the qskinny build, hence we add an explicit dependency.
    #       E.g. main.cpp -> #include <QskGraphic.cpp> -> #include "moc_QskGraphic.cpp"
    add_dependencies(${TARGET_NAME} qskinny)

    # TODO hack for standalone qvg2svg
    get_target_property(qskinny_AUTOGEN_DIR qskinny AUTOGEN_BUILD_DIR)
    if (${qskinny_AUTOGEN_DIR} STREQUAL "")
        message(FATAL_ERROR "Directory '${qskinny_AUTOGEN_DIR}' doesn't exist")
    endif()

    # TODO fix multi configuration generators
    if(CMAKE_GENERATOR MATCHES "Visual Studio.*")
        add_definitions("/I${qskinny_AUTOGEN_DIR}/include_\$(Configuration)")
    else()
        target_include_directories(${TARGET_NAME}
            PRIVATE
                ${qskinny_AUTOGEN_DIR}/include)
    endif()

    target_include_directories(${TARGET_NAME}
        PRIVATE
            ${CMAKE_SOURCE_DIR}/src/common
            ${CMAKE_SOURCE_DIR}/src/graphic)

    target_compile_definitions(${TARGET_NAME}
        PRIVATE
            QSK_STANDALONE)

    target_link_libraries(${TARGET_NAME}
        PRIVATE
            ${Qt}::Gui
            ${Qt}::GuiPrivate
            ${Qt}::Svg)
else()
    target_link_libraries(${TARGET_NAME}
        PRIVATE
            qskinny
            ${Qt}::Svg)
endif()

set_target_properties(${TARGET_NAME}
    PROPERTIES
        FOLDER tools)

install(TARGETS ${TARGET_NAME} DESTINATION ${TARGET_INSTALL_SUFFIX})
