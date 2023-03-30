set(QSK_PLUGIN_SUBDIR skins)

list(APPEND HEADERS
    QskSquiekGlobal.h
    QskSquiekSkin.h
    QskSquiekSkinFactory.h)

list(APPEND SOURCES
    QskSquiekSkin.cpp
    QskSquiekSkinFactory.cpp)

list(APPEND OTHER_FILES metadata.json)

qsk_plugin(squiekskin skins)
target_compile_definitions(squiekskin PRIVATE QSK_SQUIEK_MAKEDLL)
