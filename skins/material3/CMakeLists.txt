############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
# This file may be used under the terms of the QSkinny License, Version 1.0
############################################################################

set(SOURCES
    QskMaterial3Global.h QskMaterial3Skin.h QskMaterial3Skin.cpp
    QskMaterial3SkinFactory.h QskMaterial3SkinFactory.cpp
)
qt_add_resources(SOURCES icons.qrc)

qsk_add_plugin(material3skin skins QskMaterial3SkinFactory ${SOURCES})
target_compile_definitions(material3skin PRIVATE QSK_MATERIAL3_MAKEDLL)
