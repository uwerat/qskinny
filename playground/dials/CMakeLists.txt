CONFIG += qskexample

list(APPEND TARGET_HEADERS
    SkinFactory.h
    Dial.h
    DialSkinlet.h
    Dashboard.h

list(APPEND TARGET_SOURCES
    SkinFactory.cpp
    Dial.cpp
    DialSkinlet.cpp
    Dashboard.cpp
    main.cpp

list(APPEND TARGET_RESSOURCES
    images.qrc
