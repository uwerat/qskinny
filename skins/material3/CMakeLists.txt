set(QSK_PLUGIN_SUBDIR skins)

list(APPEND HEADERS
    QskMaterial3Global.h
    QskMaterial3Skin.h
    QskMaterial3SkinFactory.h)

list(APPEND SOURCES
    QskMaterial3Skin.cpp
    QskMaterial3SkinFactory.cpp)

list(APPEND RESSOURCES
    icons.qrc )

list(APPEND OTHER_FILES metadata.json)

qsk_plugin(material3skin skins)
