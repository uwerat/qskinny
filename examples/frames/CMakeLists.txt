CONFIG += qskexample qskqmlexport

list(APPEND TARGET_RESSOURCES frames.qrc)

    list(APPEND TARGET_HEADERS Frame.h)

        list(APPEND TARGET_SOURCES Frame.cpp main.cpp)
