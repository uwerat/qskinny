############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES GradientView.h GradientView.cpp main.cpp)

if(TARGET Qt::QuickShapesPrivate)
    list(APPEND SOURCES GradientQuickShape.h GradientQuickShape.cpp)
endif()

set(target gradients)
qsk_add_example(${target} ${SOURCES})

if(TARGET Qt::QuickShapesPrivate)
    target_compile_definitions(${target} PRIVATE SHAPE_GRADIENT)
    target_link_libraries(${target} PRIVATE Qt::QuickShapesPrivate )
endif()
