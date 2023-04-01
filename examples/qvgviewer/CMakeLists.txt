list(APPEND HEADERS
    MainWindow.h)

list(APPEND SOURCES
    MainWindow.cpp
    main.cpp)

list(APPEND RESOURCES
    qvgviewer.qrc)

qsk_add_example(qvgviewer)
