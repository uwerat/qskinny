############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
# This file may be used under the terms of the QSkinny License, Version 1.0
############################################################################

set(HEADERS QskQmlGlobal.h QskShortcutQml.h QskLayoutQml.h QskMainQml.h QskQml.h)
set(SOURCES QskShortcutQml.cpp QskLayoutQml.cpp QskMainQml.cpp QskQml.cpp)

set(target qskqmlexport)
qsk_add_library(${target} SHARED ${SOURCES} ${HEADERS})

target_link_libraries(${target} PRIVATE qskinny)
target_include_directories(${target} PUBLIC ${CMAKE_CURRENT_LIST_DIR})
set_target_properties(${target} PROPERTIES PUBLIC_HEADER QskQml.h)

if(BUILD_QSKDLL)
    set_target_properties( ${target} PROPERTIES DEFINE_SYMBOL QSK_QML_MAKEDLL )
endif()

install(TARGETS ${target} )
