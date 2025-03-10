############################################################################
# QSkinny - Copyright (C) 2023 Edelhirsch Software GmbH
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

list(APPEND HEADERS
    QskFluent2Global.h QskFluent2Theme.h QskFluent2SkinFactory.h
)

list(APPEND SOURCES
    QskFluent2Theme.cpp
    QskFluent2Skin.cpp
    QskFluent2SkinFactory.cpp
)

qt_add_resources(SOURCES QskFluent2Icons.qrc)

qsk_add_plugin(fluent2skin skins QskFluent2SkinFactory
    ${SOURCES} ${HEADERS} ${PRIVATE_HEADERS}
)

set_target_properties(fluent2skin PROPERTIES
    DEFINE_SYMBOL QSK_FLUENT2_MAKEDLL
)
