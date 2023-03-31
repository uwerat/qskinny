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

if( BUILD_QML_EXPORT )
    list(APPEND RESOURCES shapes.qrc)
endif()

qsk_example(shapes)

if( BUILD_QML_EXPORT )
    target_compile_definitions(shapes PRIVATE NO_QML)
endif()
