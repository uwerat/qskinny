############################################################################
# QSkinny - Copyright (C) 2023 Edelhirsch Software GmbH
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES
    QskFluent2Global.h QskFluent2Skin.h QskFluent2Skin.cpp
    QskFluent2SkinFactory.h QskFluent2SkinFactory.cpp
)
qt_add_resources(SOURCES icons.qrc)

qsk_add_plugin(fluent2skin skins QskFluent2SkinFactory ${SOURCES})
set_target_properties(fluent2skin PROPERTIES DEFINE_SYMBOL QSK_FLUENT2_MAKEDLL )
