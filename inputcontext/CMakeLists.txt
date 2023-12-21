############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
#           SPDX-License-Identifier: BSD-3-Clause
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
    qsk_setup_Pinyin()

    list(APPEND SOURCES
        ${QSK_SOURCE_DIR}/src/inputpanel/QskPinyinTextPredictor.h
        ${QSK_SOURCE_DIR}/src/inputpanel/QskPinyinTextPredictor.cpp
    )
endif()

if(ENABLE_HUNSPELL)
    qsk_setup_Hunspell()

    list(APPEND SOURCES
        ${QSK_SOURCE_DIR}/src/inputpanel/QskHunspellTextPredictor.h
        ${QSK_SOURCE_DIR}/src/inputpanel/QskHunspellTextPredictor.cpp
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
