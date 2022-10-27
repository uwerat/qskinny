# TODO
# CONFIG += plugin
# CONFIG += qskinny

set(TARGET_NAME qskinputcontext)

list(APPEND TARGET_HEADERS
    QskInputContextGlobal.h)

list(APPEND TARGET_SOURCES
    QskInputContextPlugin.cpp)

list(APPEND TARGET_OTHER_FILES
    metadata.json)

if(ENABLE_PINYIN)
    if(UNIX)
        # TODO CONFIG += link_pkgconfig PKGCONFIG += pinyin
        list(APPEND TARGET_HEADERS
            QskPinyinTextPredictor.h)
        list(APPEND TARGET_SOURCES
            QskPinyinTextPredictor.cpp)
    else()
        message(WARNING "Feature 'pinyin' currently only available for UNIX!")
    endif()
endif()

if(ENABLE_HUNSPELL)
    if(UNIX)
        # TODO CONFIG += link_pkgconfig PKGCONFIG += hunspell
        list(APPEND TARGET_HEADERS
            QskHunspellTextPredictor.h)
        list(APPEND TARGET_SOURCES
            QskHunspellTextPredictor.cpp)
    else()
        message(WARNING "Feature 'hunspell' currently only available for UNIX!")
    endif()
endif()

add_library(${TARGET_NAME} SHARED ${TARGET_SOURCES} ${TARGET_HEADERS} ${TARGET_OTHER_FILES})

if(BUILD_QSKDLL)
    target_compile_definitions(${TARGET_NAME}
        PRIVATE
            QSK_INPUTCONTEXT_MAKEDLL)
endif()

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        ${Qt}::GuiPrivate)

set_target_properties(${TARGET_NAME} PROPERTIES
    AUTOMOC ON)

install(TARGETS ${TARGET_NAME} DESTINATION ${QSK_INSTALL_PLUGINS}/platforminputcontexts)