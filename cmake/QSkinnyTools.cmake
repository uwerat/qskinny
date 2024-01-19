############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

## @param SVG_FILENAME absolute filename to the svg
## @param QVG_FILENAME absolute filename to the qvg
function(qsk_svg2qvg SVG_FILENAME QVG_FILENAME)
    get_filename_component(QVG_FILENAME ${QVG_FILENAME} ABSOLUTE)
    get_filename_component(SVG_FILENAME ${SVG_FILENAME} ABSOLUTE)
    add_custom_command(
        COMMAND svg2qvg ${SVG_FILENAME} ${QVG_FILENAME}
        OUTPUT ${QVG_FILENAME}
        DEPENDS ${SVG_FILENAME}
        WORKING_DIRECTORY $<TARGET_FILE_DIR:Qt${QT_VERSION_MAJOR}::Svg>
        COMMENT "Compiling ${SVG_FILENAME} to ${QVG_FILENAME}")
endfunction()

