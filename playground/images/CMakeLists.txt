CONFIG += qskexample qskqmlexport

target_link_libraries(${TARGET_NAME}
    PRIVATE
        Qt5::QuickPrivate)

list(APPEND TARGET_RESSOURCES
    images.qrc

list(APPEND TARGET_HEADERS
    Image.h

list(APPEND TARGET_SOURCES
    Image.cpp
    main.cpp
