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

    get_target_property(qskinny_AUTOGEN_DIR qskinny AUTOGEN_BUILD_DIR)
    if (EXISTS ${qskinny_AUTOGEN_DIR})
    else()
        set(qskinny_AUTOGEN_DIR ${CMAKE_BINARY_DIR}/src/qskinny_autogen/include_$<CONFIG>) # TODO
    endif()

    target_include_directories(${TARGET_NAME}
        PRIVATE
            ${CMAKE_SOURCE_DIR}/src/common
            ${CMAKE_SOURCE_DIR}/src/graphic
            ${qskinny_AUTOGEN_DIR})

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

install(TARGETS ${TARGET_NAME} DESTINATION ${TARGET_INSTALL_SUFFIX})
