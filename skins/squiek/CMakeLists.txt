set(QSK_PLUGIN_SUBDIR skins)

list(APPEND HEADERS
    QskSquiekGlobal.h
    QskSquiekSkin.h
    QskSquiekSkinFactory.h)

list(APPEND SOURCES
    QskSquiekSkin.cpp
    QskSquiekSkinFactory.cpp)

qsk_add_plugin(squiekskin skins QskSquiekSkinFactory)
target_compile_definitions(squiekskin PRIVATE QSK_SQUIEK_MAKEDLL)
