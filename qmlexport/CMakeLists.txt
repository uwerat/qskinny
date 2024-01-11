############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(HEADERS
    QskQmlGlobal.h
    QskShortcutQml.h
    QskLayoutQml.h
    QskMainQml.h
    QskQml.h)

set(SOURCES
    QskShortcutQml.cpp
    QskLayoutQml.cpp
    QskMainQml.cpp
    QskQml.cpp)

set(target qskqmlexport)
qsk_add_library(${target} SHARED ${SOURCES} ${HEADERS})

target_link_libraries(${target} PRIVATE qskinny)
target_include_directories(${target} PUBLIC 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}>)
set_target_properties(${target} PROPERTIES PUBLIC_HEADER QskQml.h)

if(BUILD_QSKDLL)
    set_target_properties(${target} PROPERTIES DEFINE_SYMBOL QSK_QML_MAKEDLL)
    set_target_properties(${target} PROPERTIES INSTALL_RPATH "\${ORIGIN}" )
    set_target_properties(${target} PROPERTIES
        VERSION ${CMAKE_PROJECT_VERSION} SOVERSION ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR} )
endif()

# packaging
set(PACKAGE_NAME            QmlExport)
set(PACKAGE_TARGET_FILENAME QSkinny${PACKAGE_NAME}Targets.cmake)
set(QSKQE_INSTALL_HEADERS   "${CMAKE_INSTALL_INCLUDEDIR}/${target}")

set_target_properties(${target} PROPERTIES FOLDER libs)
set_target_properties(${target} PROPERTIES EXPORT_NAME QmlExport)

install(TARGETS ${target} EXPORT ${PACKAGE_TARGET_FILENAME}
    LIBRARY DESTINATION       ${QSK_INSTALL_LIBS}
    ARCHIVE DESTINATION       ${QSK_INSTALL_LIBS}
    RUNTIME DESTINATION       ${QSK_INSTALL_LIBS}
    INCLUDES DESTINATION      ${QSK_INSTALL_HEADERS}
    PUBLIC_HEADER DESTINATION ${QSK_INSTALL_HEADERS})

export(EXPORT ${PACKAGE_TARGET_FILENAME}
    NAMESPACE ${PACKAGE_NAMESPACE})

install(EXPORT ${PACKAGE_TARGET_FILENAME}
    FILE
        ${PACKAGE_TARGET_FILENAME}
    NAMESPACE
        ${PACKAGE_NAMESPACE}
    DESTINATION
        ${PACKAGE_LOCATION})

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    install(FILES $<TARGET_PDB_FILE:${target}> 
        DESTINATION
            ${QSK_INSTALL_LIBS} OPTIONAL)
endif()