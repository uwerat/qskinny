############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES GradientView.h GradientView.cpp main.cpp)

if(TARGET quickshapes_private)
    list(APPEND SOURCES GradientQuickShape.h GradientQuickShape.cpp)
endif()

qsk_add_example(gradients ${SOURCES})

if(TARGET quickshapes_private)
    target_compile_definitions(gradients PRIVATE SHAPE_GRADIENT)
    target_link_libraries(gradients PRIVATE quickshapes_private)
endif()
