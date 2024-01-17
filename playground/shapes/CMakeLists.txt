############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(SOURCES
    ShapeItem.h ShapeItem.cpp
    GeometricShape.h GeometricShape.cpp
    Stroke.h Stroke.cpp
    Window.h Window.cpp
    main.cpp
)

if( BUILD_QML_EXPORT )
    qt_add_resources(SOURCES shapes.qrc)
endif()

qsk_add_example(shapes ${SOURCES})

if( NOT BUILD_QML_EXPORT )
    target_compile_definitions(shapes PRIVATE NO_QML)
endif()
