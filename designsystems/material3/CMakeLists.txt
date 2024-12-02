############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

list(APPEND HEADERS
    QskMaterial3Global.h QskMaterial3Skin.h QskMaterial3SkinFactory.h
)

list(APPEND PRIVATE_HEADERS
    QskMaterial3ProgressBarSkinlet.h
    QskMaterial3SliderSkinlet.h
    QskMaterial3TextFieldSkinlet.h
)

list(APPEND SOURCES
    QskMaterial3Skin.cpp
    QskMaterial3SkinFactory.cpp
    QskMaterial3ProgressBarSkinlet.cpp
    QskMaterial3SliderSkinlet.cpp
    QskMaterial3TextFieldSkinlet.cpp
)

qt_add_resources(SOURCES QskMaterial3Icons.qrc)

qsk_add_plugin(material3skin skins QskMaterial3SkinFactory
    ${SOURCES} ${HEADERS} ${PRIVATE_HEADERS}
)

set_target_properties(material3skin PROPERTIES
    DEFINE_SYMBOL QSK_MATERIAL3_MAKEDLL
)
