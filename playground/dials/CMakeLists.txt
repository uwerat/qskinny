list(APPEND RESOURCES images.qrc)

list(APPEND HEADERS
    SkinFactory.h
    Dial.h
    DialSkinlet.h
    Dashboard.h)

list(APPEND SOURCES
    SkinFactory.cpp
    Dial.cpp
    DialSkinlet.cpp
    Dashboard.cpp
    main.cpp)

qsk_add_example(dials)
