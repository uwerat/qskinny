set(TARGET_NAME qskinputcontext)

list(APPEND TARGET_HEADERS
    QskInputContextGlobal.h)

list(APPEND TARGET_SOURCES
    QskInputContextPlugin.cpp)

list(APPEND TARGET_OTHER_FILES
    metadata.json)

if(ENABLE_PINYIN)
    list(APPEND TARGET_HEADERS
        ${CMAKE_SOURCE_DIR}/src/inputpanel/QskPinyinTextPredictor.h)
    list(APPEND TARGET_SOURCES
        ${CMAKE_SOURCE_DIR}/src/inputpanel/QskPinyinTextPredictor.cpp)
endif()

if(ENABLE_HUNSPELL)
    list(APPEND TARGET_HEADERS
        ${CMAKE_SOURCE_DIR}/src/inputpanel/QskHunspellTextPredictor.h)
    list(APPEND TARGET_SOURCES
        ${CMAKE_SOURCE_DIR}/src/inputpanel/QskHunspellTextPredictor.cpp)
endif()

qt_add_library(${TARGET_NAME} SHARED ${TARGET_SOURCES} ${TARGET_HEADERS} ${TARGET_OTHER_FILES})

if(ENABLE_PINYIN)
    target_link_libraries(${TARGET_NAME}
        PRIVATE
            pinyin
            Fcitx5::Utils)
endif()

if(ENABLE_HUNSPELL)
    target_link_libraries(${TARGET_NAME}
        PRIVATE
            hunspell)
endif()

if(BUILD_QSKDLL)
    target_compile_definitions(${TARGET_NAME}
        PRIVATE
            QSK_INPUTCONTEXT_MAKEDLL)
endif()

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        Qt::GuiPrivate)

set_target_properties(${TARGET_NAME} PROPERTIES
    AUTOMOC ON)

install(TARGETS ${TARGET_NAME} DESTINATION ${QSK_INSTALL_PLUGINS}/platforminputcontexts)