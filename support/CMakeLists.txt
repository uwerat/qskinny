# TODO
# CONFIG += ensure_skins 
# CONFIG += fontconfig
# CONFIG += qskinny

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

# TODO
#fontconfig{
#
#    QSK_FONTDIR = $${PWD} / fonts QSK_FONTCACHEDIR =
#                      $${OUT_PWD} / fontconfig QSK_FONTCONF_FILE =
#                          $${QSK_FONTCACHEDIR} / fonts.conf
#
#                                                       QSK_FONTCONF_DATA =
#                              "$$cat($${PWD}/fonts.conf.in, false)" QSK_FONTCONF_DATA = $$replace(
#                                  QSK_FONTCONF_DATA, FONTCACHEDIR, $${QSK_FONTCACHEDIR})
#                                  QSK_FONTCONF_DATA = $$replace(
#                                      QSK_FONTCONF_DATA, FONTDIR, $${QSK_FONTDIR})
#                                      write_file($${QSK_FONTCONF_FILE}, QSK_FONTCONF_DATA)
#
#                                          DEFINES +=
#    FONTCONFIG_FILE = $$clean_path($$QSK_FONTCONF_FILE)
#}

set(TARGET_NAME qsktestsupport)

if (BUILD_QSKDLL)
    add_library(${TARGET_NAME} SHARED ${TARGET_SOURCES} ${TARGET_HEADERS})
else()
    add_library(${TARGET_NAME} STATIC ${TARGET_SOURCES} ${TARGET_HEADERS})
endif()

set_target_properties(${TARGET_NAME} PROPERTIES
   AUTOMOC ON)

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

option(ENABLE_ENSURE_SKINS "Enabling fall back code, that inserts some skin factories manually when not finding skin factories as plugins" ON)

if(ENABLE_ENSURE_SKINS)
    target_include_directories(${TARGET_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/skins)
    target_compile_definitions(${TARGET_NAME} PRIVATE ENSURE_SKINS)

    # TODO
    # use_install_rpath: QMAKE_RPATHDIR *= $${QSK_INSTALL_PLUGINS}/skins
    # use_local_rpath: QMAKE_RPATHDIR *= $${QSK_PLUGIN_DIR}/skins
    target_link_libraries(${TARGET_NAME}
        PRIVATE
            squiekskin
            material3skin)
endif()

install(TARGETS ${TARGET_NAME} DESTINATION lib) # TODO unify lib install directory