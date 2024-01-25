############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES
    QskFusionGlobal.h
    QskFusionPalette.h QskFusionPalette.cpp
    QskFusionSkin.h QskFusionSkin.cpp
    QskFusionSkinFactory.h QskFusionSkinFactory.cpp
)

qsk_add_plugin(fusionskin skins QskFusionSkinFactory ${SOURCES})
set_target_properties(fusionskin PROPERTIES DEFINE_SYMBOL QSK_FUSION_MAKEDLL)
