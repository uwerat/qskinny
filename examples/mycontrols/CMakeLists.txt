list(APPEND SOURCES
    MySkin.cpp
    MyToggleButton.cpp
    MyToggleButtonSkinlet.cpp
    main.cpp)

list(APPEND HEADERS
    MyToggleButton.h
    MyToggleButtonSkinlet.h
    MySkin.h)

qsk_add_example(mycontrols)
