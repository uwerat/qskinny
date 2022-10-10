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

add_library(${TARGET_NAME} SHARED ${TARGET_SOURCES} ${TARGET_HEADERS} ${TARGET_OTHER_FILES})

if(BUILD_QSKDLL)
    target_compile_definitions(${TARGET_NAME}
        PRIVATE
            QSK_INPUTCONTEXT_MAKEDLL)
endif()

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        Qt5::GuiPrivate)

set_target_properties(${TARGET_NAME} PROPERTIES
    AUTOMOC ON)

install(TARGETS ${TARGET_NAME} DESTINATION ${QSK_INSTALL_PLUGINS}/platforminputcontexts)

# TODO
# pinyin {
#     unix
#     {
#         CONFIG += link_pkgconfig PKGCONFIG += pinyin
# 
#             HEADERS += QskPinyinTextPredictor.h
# 
#                            SOURCES += QskPinyinTextPredictor.cpp
#    }
# }
# 
# hunspell {
#     unix
#     {
#         CONFIG += link_pkgconfig PKGCONFIG += hunspell
# 
#             HEADERS += QskHunspellTextPredictor.h
# 
#                            SOURCES += QskHunspellTextPredictor.cpp
#    }
# 
# }