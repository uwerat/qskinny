############################################################################
# QSkinny - Copyright (C) 2023 Edelhirsch Software GmbH
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES
    QskFluent2Global.h
    QskFluent2Theme.h QskFluent2Theme.cpp
    QskFluent2Skin.h QskFluent2Skin.cpp
    QskFluent2SkinFactory.h QskFluent2SkinFactory.cpp
    QskFluent2TextInputSkinlet.h QskFluent2TextInputSkinlet.cpp
)
qt_add_resources(SOURCES QskFluent2Icons.qrc)

qsk_add_plugin(fluent2skin skins QskFluent2SkinFactory ${SOURCES})
set_target_properties(fluent2skin PROPERTIES DEFINE_SYMBOL QSK_FLUENT2_MAKEDLL )
