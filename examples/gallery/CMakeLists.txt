list(APPEND TARGET_HEADERS
    label/LabelPage.h
    inputs/InputPage.h
    progressbar/ProgressBarPage.h
    button/ButtonPage.h
    selector/SelectorPage.h
    dialog/DialogPage.h
    Page.h)

list(APPEND TARGET_SOURCES
    label/LabelPage.cpp
    inputs/InputPage.cpp
    progressbar/ProgressBarPage.cpp
    button/ButtonPage.cpp
    selector/SelectorPage.cpp
    dialog/DialogPage.cpp
    Page.cpp
    main.cpp)

set(TARGET_NAME gallery)

qt_add_executable(${TARGET_NAME} WIN32 MACOSX_BUNDLE ${TARGET_SOURCES} ${TARGET_HEADERS})

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
