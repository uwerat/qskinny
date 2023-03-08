list(APPEND TARGET_HEADERS
    GradientView.h)

list(APPEND TARGET_SOURCES
    GradientView.cpp
    main.cpp)

set(TARGET_NAME gradients)

qt_add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE
    ${TARGET_HEADERS}
    ${TARGET_SOURCES})

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        qsktestsupport)

if(TARGET quickshapes_private)
    target_sources(gradients PUBLIC
        GradientQuickShape.cpp GradientQuickShape.h
    )

    target_compile_definitions(gradients PRIVATE
        SHAPE_GRADIENT
    )

    target_link_libraries(gradients PRIVATE
        quickshapes_private
    )
endif()

set_target_properties(${TARGET_NAME}
    PROPERTIES
        AUTOMOC ON
        FOLDER playground)
