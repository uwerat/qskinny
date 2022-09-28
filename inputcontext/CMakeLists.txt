set(TARGET_NAME qskinputcontext)

target_link_libraries(${TARGET_NAME} PRIVATE gui-private)

# TODO
# CONFIG += plugin
# CONFIG += qskinny

if(QskDll)
    target_compile_definitions(${TARGET_NAME} PRIVATE QSK_INPUTCONTEXT_MAKEDLL)
endif()

list(APPEND TARGET_HEADERS
    QskInputContextGlobal.h)

list(APPEND TARGET_SOURCES
    QskInputContextPlugin.cpp)

list(APPEND TARGET_OTHER_FILES
    metadata.json)

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

add_library(${TARGET_NAME} SHARED ${TARGET_SOURCES} ${TARGET_HEADERS} ${TARGET_OTHER_FILES})

install(TARGETS ${TARGET_NAME} DESTINATION ${QSK_INSTALL_PLUGINS}/platforminputcontexts)