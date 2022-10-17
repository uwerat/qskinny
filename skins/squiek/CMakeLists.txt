set(QSK_PLUGIN_SUBDIR skins)

set(TARGET_NAME squiekskin)

list(APPEND TARGET_HEADERS
    QskSquiekGlobal.h
    QskSquiekSkin.h
    QskSquiekSkinFactory.h)

list(APPEND TARGET_SOURCES
    QskSquiekSkin.cpp
    QskSquiekSkinFactory.cpp)

list(APPEND TARGET_OTHER_FILES
    metadata.json)

if(BUILD_QSKDLL)
    add_library(${TARGET_NAME} SHARED ${TARGET_HEADERS} ${TARGET_SOURCES} ${TARGET_OTHER_FILES})
else()
    add_library(${TARGET_NAME} STATIC ${TARGET_HEADERS} ${TARGET_SOURCES} ${TARGET_OTHER_FILES})
endif()

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny)

if(BUILD_QSKDLL)
    target_compile_definitions(${TARGET_NAME}
        PRIVATE
            QSK_SQUIEK_MAKEDLL)
endif()

set_target_properties(${TARGET_NAME} PROPERTIES
    AUTOMOC ON
    FOLDER skins)

install(TARGETS ${TARGET_NAME} DESTINATION ${QSK_INSTALL_PLUGINS}/${QSK_PLUGIN_SUBDIR})
