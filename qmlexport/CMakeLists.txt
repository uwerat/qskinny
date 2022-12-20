set(TARGET_NAME qskqmlexport)

list(APPEND TARGET_HEADERS
    QskQmlGlobal.h
    QskShortcutQml.h
    QskLayoutQml.h
    QskMainQml.h
    QskQml.h)

list(APPEND TARGET_SOURCES
    QskShortcutQml.cpp
    QskLayoutQml.cpp
    QskMainQml.cpp
    QskQml.cpp)

qt_add_library(${TARGET_NAME} SHARED ${TARGET_SOURCES} ${TARGET_HEADERS})

set_target_properties(${TARGET_NAME}
    PROPERTIES
        AUTOMOC ON)

target_link_libraries(${TARGET_NAME}
    PRIVATE
    qskinny)

target_include_directories(${TARGET_NAME}
   PUBLIC
       ${CMAKE_CURRENT_LIST_DIR})

if(BUILD_QSKDLL)
    target_compile_definitions(${TARGET_NAME}
        PRIVATE
            QSK_QML_MAKEDLL)
endif()

install(TARGETS ${TARGET_NAME} DESTINATION qmlexport)