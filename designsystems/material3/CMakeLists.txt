############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES
    QskMaterial3Global.h QskMaterial3Skin.h QskMaterial3Skin.cpp
    QskMaterial3SkinFactory.h QskMaterial3SkinFactory.cpp
    QskMaterial3TextInputSkinlet.h QskMaterial3TextInputSkinlet.cpp
)
qt_add_resources(SOURCES QskMaterial3Icons.qrc)

qsk_add_plugin(material3skin skins QskMaterial3SkinFactory ${SOURCES})
set_target_properties(material3skin PROPERTIES DEFINE_SYMBOL QSK_MATERIAL3_MAKEDLL )
