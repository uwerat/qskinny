list(APPEND TARGET_HEADERS
    label/LabelPage.h)

list(APPEND TARGET_SOURCES
    label/LabelPage.cpp)

list(APPEND TARGET_HEADERS
    slider/SliderPage.h)

list(APPEND TARGET_SOURCES
    slider/SliderPage.cpp)

list(APPEND TARGET_HEADERS
    progressbar/ProgressBarPage.h)

list(APPEND TARGET_SOURCES
    progressbar/ProgressBarPage.cpp)

list(APPEND TARGET_HEADERS
    button/ButtonPage.h)

list(APPEND TARGET_SOURCES
    button/ButtonPage.cpp)

list(APPEND TARGET_HEADERS
    textinput/TextInputPage.h)

list(APPEND TARGET_SOURCES
    textinput/TextInputPage.cpp)

list(APPEND TARGET_HEADERS
    selector/SelectorPage.h)

list(APPEND TARGET_SOURCES
    selector/SelectorPage.cpp)

list(APPEND TARGET_HEADERS
    dialog/DialogPage.h)

list(APPEND TARGET_SOURCES
    dialog/DialogPage.cpp)

list(APPEND TARGET_HEADERS
    Page.h)

list(APPEND TARGET_SOURCES
    Page.cpp
    main.cpp)

set(TARGET_NAME gallery)

add_executable(${TARGET_NAME} ${TARGET_SOURCES} ${TARGET_HEADERS})

set_target_properties(${TARGET_NAME}
    PROPERTIES
        AUTOMOC ON
        FOLDER examples)

target_link_libraries(${TARGET_NAME}
    PRIVATE
        qskinny
        qsktestsupport)

target_include_directories(${TARGET_NAME}
   PRIVATE
       ${CMAKE_CURRENT_LIST_DIR})