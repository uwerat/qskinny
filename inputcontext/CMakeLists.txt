set(TARGET_NAME qskinputcontext)

list(APPEND HEADERS QskInputContextGlobal.h)
list(APPEND SOURCES QskInputContextPlugin.cpp)

if(NOT LINUX AND ENABLE_HUNSPELL)
    message(WARNING "Feature 'hunspell' only available for UNIX!")
    unset(ENABLE_HUNSPELL CACHE)
endif()

if(NOT LINUX AND ENABLE_PINYIN)
    message(WARNING "Feature 'pinyin' only available for UNIX!")
    unset(ENABLE_PINYIN CACHE)
endif()

if(ENABLE_PINYIN)
    include("QSkinnyOptionalPinyin")

    list(APPEND HEADERS
        ${CMAKE_SOURCE_DIR}/src/inputpanel/QskPinyinTextPredictor.h)
    list(APPEND SOURCES
        ${CMAKE_SOURCE_DIR}/src/inputpanel/QskPinyinTextPredictor.cpp)
endif()

if(ENABLE_HUNSPELL)
    include("QSkinnyOptionalHunspell")

    list(APPEND HEADERS
        ${CMAKE_SOURCE_DIR}/src/inputpanel/QskHunspellTextPredictor.h)
    list(APPEND SOURCES
        ${CMAKE_SOURCE_DIR}/src/inputpanel/QskHunspellTextPredictor.cpp)
endif()

qsk_add_plugin(qskinputcontext platforminputcontexts QskInputContextPlugin)

if(ENABLE_PINYIN)
    target_link_libraries(qskinputcontext PRIVATE pinyin Fcitx5::Utils)
endif()

if(ENABLE_HUNSPELL)
    target_link_libraries(qskinputcontext PRIVATE hunspell)
endif()

target_compile_definitions(qskinputcontext PRIVATE QSK_INPUTCONTEXT_MAKEDLL)
target_link_libraries(qskinputcontext PRIVATE qskinny Qt::GuiPrivate)
