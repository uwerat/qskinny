CONFIG += qskexample
target_link_libraries(${TARGET_NAME} PRIVATE webengine webengine-private webenginecore-private core-private)

list(APPEND TARGET_SOURCES
    main.cpp
