set(TARGET_NAME svg2qvg)
set(TARGET_INSTALL_SUFFIX bin/tools)
set(TARGET_SOURCES main.cpp)

#TODO should this be configurable globally ?
option(${TARGET_NAME}_STANDALONE "If 'ON' application won't be linked against qskinny library" ON)

add_executable(${TARGET_NAME} ${TARGET_SOURCES})

if(${TARGET_NAME}_STANDALONE)

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
            Qt5::Gui
            Qt5::GuiPrivate
            Qt5::Svg)
else()
    target_compile_definitions(${TARGET_NAME}
        PRIVATE
            QskDll)

    target_link_libraries(${TARGET_NAME}
        PRIVATE
            qskinny
            Qt5::Svg)
endif()

install(TARGETS ${TARGET_NAME} DESTINATION ${TARGET_INSTALL_SUFFIX})
