############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
# This file may be used under the terms of the QSkinny License, Version 1.0
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
        WORKING_DIRECTORY $<TARGET_FILE_DIR:${Qt}::Svg>
        COMMENT "Compiling ${SVG_FILENAME} to ${QVG_FILENAME}")
endfunction()
