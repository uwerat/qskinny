############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

set(target glyph2qvg)
qsk_add_executable(${target} main.cpp)

if(BUILD_TOOLS_STANDALONE)
    qsk_embed_sources(${target})
else()
    target_link_libraries(${target} PRIVATE qskinny)
endif()

set_target_properties(${target} PROPERTIES FOLDER tools)

install(TARGETS ${target})

# packaging TODO ...
