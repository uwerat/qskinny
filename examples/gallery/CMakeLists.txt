list(APPEND RESOURCES icons.qrc )

list(APPEND HEADERS
    label/LabelPage.h
    inputs/InputPage.h
    progressbar/ProgressBarPage.h
    button/ButtonPage.h
    selector/SelectorPage.h
    dialog/DialogPage.h
    Page.h)

list(APPEND SOURCES
    label/LabelPage.cpp
    inputs/InputPage.cpp
    progressbar/ProgressBarPage.cpp
    button/ButtonPage.cpp
    selector/SelectorPage.cpp
    dialog/DialogPage.cpp
    Page.cpp
    main.cpp)

qsk_add_example(gallery)
