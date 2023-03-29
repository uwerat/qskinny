list(APPEND TARGET_HEADERS
    SkinnyGlobal.h
    SkinnyNamespace.h
    SkinnyShapeFactory.h
    SkinnyShapeProvider.h
    SkinnyShortcut.h)

list(APPEND TARGET_SOURCES
    SkinnyNamespace.cpp
    SkinnyShapeFactory.cpp
    SkinnyShapeProvider.cpp
    SkinnyShortcut.cpp)

set(TARGET_NAME qsktestsupport)

if (BUILD_QSKDLL)
    qt_add_library(${TARGET_NAME} SHARED ${TARGET_SOURCES} ${TARGET_HEADERS})
else()
    qt_add_library(${TARGET_NAME} STATIC ${TARGET_SOURCES} ${TARGET_HEADERS})
endif()

if(ENABLE_FONT_CONFIG)
    set(QSK_FONTDIR       ${CMAKE_CURRENT_LIST_DIR}/fonts)
    set(QSK_FONTCACHEDIR  ${CMAKE_CURRENT_BINARY_DIR}/fontconfig)
    set(QSK_FONTCONF_FILE ${QSK_FONTCACHEDIR}/fonts.conf)

    configure_file(fonts.conf.cmake.in ${QSK_FONTCONF_FILE}
        @ONLY
        NEWLINE_STYLE LF)

    target_compile_definitions(${TARGET_NAME}
        PRIVATE
            FONTCONFIG_FILE=${QSK_FONTCONF_FILE})
endif()

target_link_libraries(${TARGET_NAME}
   PUBLIC
       qskinny)

if (BUILD_QSKDLL)
    target_compile_definitions(${TARGET_NAME}
        PRIVATE
            SKINNY_MAKEDLL)
endif()

target_compile_definitions(${TARGET_NAME}
    PRIVATE
        PLUGIN_PATH=${CMAKE_INSTALL_PREFIX}/skins)

target_include_directories(${TARGET_NAME}
   PUBLIC
       ${CMAKE_CURRENT_LIST_DIR})

if(ENABLE_ENSURE_SKINS)
    target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/skins)
    target_compile_definitions(${TARGET_NAME} PRIVATE ENSURE_SKINS)

    target_link_libraries(${TARGET_NAME}
        PRIVATE
            squiekskin
            material3skin)
endif()

install(TARGETS ${TARGET_NAME} DESTINATION ${QSK_INSTALL_LIBS})
