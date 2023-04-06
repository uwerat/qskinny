############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

qsk_add_plugin(squiekskin skins QskSquiekSkinFactory
    QskSquiekGlobal.h QskSquiekSkin.h QskSquiekSkin.cpp
    QskSquiekSkinFactory.h QskSquiekSkinFactory.cpp
)
set_target_properties(squiekskin PROPERTIES DEFINE_SYMBOL QSK_SQUIEK_MAKEDLL)
