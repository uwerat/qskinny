set(QSK_PLUGIN_SUBDIR skins)

set(TARGET_NAME material3skin)

list(APPEND TARGET_HEADERS
    QskMaterial3Global.h
    QskMaterial3Skin.h
    QskMaterial3SkinFactory.h)

list(APPEND TARGET_SOURCES
    QskMaterial3Skin.cpp
    QskMaterial3SkinFactory.cpp)

list(APPEND TARGET_RESSOURCES
    icons.qrc )

list(APPEND TARGET_OTHER_FILES 
    metadata.json)

if(BUILD_QSKDLL)
    qt_add_library(${TARGET_NAME} SHARED ${TARGET_HEADERS} ${TARGET_SOURCES} ${TARGET_RESSOURCES} ${TARGET_OTHER_FILES})
else()
    qt_add_library(${TARGET_NAME} STATIC ${TARGET_HEADERS} ${TARGET_SOURCES} ${TARGET_RESSOURCES} ${TARGET_OTHER_FILES})
endif()

target_link_libraries(${TARGET_NAME} PRIVATE qskinny)

if(BUILD_QSKDLL)
    target_compile_definitions(${TARGET_NAME}
        PRIVATE
            QSK_MATERIAL3_MAKEDLL)
endif()

set_target_properties(${TARGET_NAME} PROPERTIES
    FOLDER skins)

install(TARGETS ${TARGET_NAME} DESTINATION ${QSK_INSTALL_PLUGINS}/${QSK_PLUGIN_SUBDIR})
