set(TARGET_NAME qskinputcontext)

list(APPEND HEADERS QskInputContextGlobal.h)
list(APPEND SOURCES QskInputContextPlugin.cpp)
list(APPEND OTHER_FILES metadata.json)

if(ENABLE_PINYIN)
    list(APPEND HEADERS
        ${CMAKE_SOURCE_DIR}/src/inputpanel/QskPinyinTextPredictor.h)
    list(APPEND SOURCES
        ${CMAKE_SOURCE_DIR}/src/inputpanel/QskPinyinTextPredictor.cpp)
endif()

if(ENABLE_HUNSPELL)
    list(APPEND HEADERS
        ${CMAKE_SOURCE_DIR}/src/inputpanel/QskHunspellTextPredictor.h)
    list(APPEND SOURCES
        ${CMAKE_SOURCE_DIR}/src/inputpanel/QskHunspellTextPredictor.cpp)
endif()

qsk_plugin(qskinputcontext platforminputcontexts)

if(ENABLE_PINYIN)
    target_link_libraries(qskinputcontext PRIVATE pinyin Fcitx5::Utils)
endif()

if(ENABLE_HUNSPELL)
    target_link_libraries(qskinputcontext PRIVATE hunspell)
endif()

target_compile_definitions(qskinputcontext PRIVATE QSK_INPUTCONTEXT_MAKEDLL)
target_link_libraries(qskinputcontext PRIVATE qskinny Qt::GuiPrivate)
