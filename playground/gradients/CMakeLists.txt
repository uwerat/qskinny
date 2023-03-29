list(APPEND HEADERS GradientView.h)
list(APPEND SOURCES GradientView.cpp main.cpp)

if(TARGET quickshapes_private)

    list(APPEND SOURCES GradientQuickShape.cpp)
    list(APPEND HEADERS GradientQuickShape.cpp)

endif()

qsk_example(gradients)

if(TARGET quickshapes_private)
    target_compile_definitions(gradients PRIVATE SHAPE_GRADIENT)
    target_link_libraries(gradients PRIVATE quickshapes_private)
endif()
