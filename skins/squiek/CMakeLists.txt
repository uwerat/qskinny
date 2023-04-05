############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
# This file may be used under the terms of the QSkinny License, Version 1.0
############################################################################

qsk_add_plugin(squiekskin skins QskSquiekSkinFactory
    QskSquiekGlobal.h QskSquiekSkin.h QskSquiekSkin.cpp
    QskSquiekSkinFactory.h QskSquiekSkinFactory.cpp
)
set_target_properties(squiekskin PROPERTIES DEFINE_SYMBOL QSK_SQUIEK_MAKEDLL)
