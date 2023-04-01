set(QSK_PLUGIN_SUBDIR skins)

list(APPEND HEADERS
    QskMaterial3Global.h
    QskMaterial3Skin.h
    QskMaterial3SkinFactory.h)

list(APPEND SOURCES
    QskMaterial3Skin.cpp
    QskMaterial3SkinFactory.cpp)

list(APPEND RESSOURCES icons.qrc )

qsk_add_plugin(material3skin skins QskMaterial3SkinFactory)
target_compile_definitions(material3skin PRIVATE QSK_MATERIAL3_MAKEDLL)
