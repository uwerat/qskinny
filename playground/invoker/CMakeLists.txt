############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

qsk_add_example(invoker Callback.h Callback.cpp Invoker.h Invoker.cpp main.cpp)
set_target_properties(invoker PROPERTIES AUTOMOC_MOC_OPTIONS --no-warnings)

