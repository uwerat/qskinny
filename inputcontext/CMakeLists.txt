############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
# This file may be used under the terms of the QSkinny License, Version 1.0
############################################################################

if(NOT LINUX AND ENABLE_HUNSPELL)
    message(WARNING "Feature 'hunspell' only available for UNIX!")
    unset(ENABLE_HUNSPELL CACHE)
endif()

if(NOT LINUX AND ENABLE_PINYIN)
    message(WARNING "Feature 'pinyin' only available for UNIX!")
    unset(ENABLE_PINYIN CACHE)
endif()

set(SOURCES QskInputContextGlobal.h QskInputContextPlugin.cpp)

if(ENABLE_PINYIN)
    include("QSkinnyOptionalPinyin")

    list(APPEND SOURCES
        ${CMAKE_SOURCE_DIR}/src/inputpanel/QskPinyinTextPredictor.h
        ${CMAKE_SOURCE_DIR}/src/inputpanel/QskPinyinTextPredictor.cpp
    )
endif()

if(ENABLE_HUNSPELL)
    include("QSkinnyOptionalHunspell")

    list(APPEND SOURCES
        ${CMAKE_SOURCE_DIR}/src/inputpanel/QskHunspellTextPredictor.h
        ${CMAKE_SOURCE_DIR}/src/inputpanel/QskHunspellTextPredictor.cpp
    )
endif()

set(target qskinputcontext)

qsk_add_plugin(${target} platforminputcontexts QskInputContextPlugin ${SOURCES})

if(ENABLE_PINYIN)
    target_link_libraries(${target} PRIVATE pinyin Fcitx5::Utils)
endif()

if(ENABLE_HUNSPELL)
    target_link_libraries(${target} PRIVATE hunspell)
endif()

set_target_properties(${target} PROPERTIES DEFINE_SYMBOL QSK_INPUTCONTEXT_MAKEDLL )
target_link_libraries(${target} PRIVATE qskinny Qt::GuiPrivate)
