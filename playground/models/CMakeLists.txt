############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

qsk_add_example(models QskModelObjectBinder.cpp QskModelObjectBinder.h  Window.h Window.cpp  main.cpp)

target_link_libraries(models PRIVATE Qt::Sql)