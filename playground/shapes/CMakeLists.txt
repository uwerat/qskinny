list(APPEND RESOURCES shapes.qrc)

list(APPEND HEADERS
    ShapeItem.h
    GeometricShape.h
    Window.h
    Stroke.h)

list(APPEND SOURCES
    Stroke.cpp
    ShapeItem.cpp
    GeometricShape.cpp
    Window.cpp
    main.cpp)

qsk_example(shapes)
