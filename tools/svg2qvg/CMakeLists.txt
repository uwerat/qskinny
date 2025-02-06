############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(target svg2qvg)
qsk_add_executable(${target} main.cpp)

if(BUILD_TOOLS_STANDALONE)
    qsk_embed_sources(${target})
else()
    target_link_libraries(${target} PRIVATE qskinny)
endif()

target_link_libraries(${target} PRIVATE Qt::Svg)

set_target_properties(${target} PROPERTIES FOLDER tools)

install(TARGETS ${target})

# packaging
set(PACKAGE_TARGET_FILENAME QSkinnyToolsTargets.cmake)

set_target_properties(${target} PROPERTIES EXPORT_NAME Svg2Qvg)
set_target_properties(${target} PROPERTIES INSTALL_RPATH_USE_LINK_PATH TRUE)

install(TARGETS ${target} EXPORT ${PACKAGE_TARGET_FILENAME})

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
            ${CMAKE_INSTALL_BINDIR} OPTIONAL)
endif()
