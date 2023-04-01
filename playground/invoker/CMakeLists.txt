############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
# This file may be used under the terms of the 3-clause BSD License
############################################################################

qsk_add_example(invoker Callback.h Callback.cpp Invoker.h Invoker.cpp main.cpp)
set_target_properties(invoker PROPERTIES AUTOMOC_MOC_OPTIONS --no-warnings)

