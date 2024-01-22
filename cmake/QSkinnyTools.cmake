############################################################################
# QSkinny - Copyright (C) The authors
#           SPDX-License-Identifier: BSD-3-Clause
############################################################################

## @param SVG_FILENAME absolute filename to the svg
## @param QVG_FILENAME absolute filename to the qvg
function(qsk_svg2qvg SVG_FILENAME QVG_FILENAME)
    get_filename_component(QVG_FILENAME ${QVG_FILENAME} ABSOLUTE)
    get_filename_component(SVG_FILENAME ${SVG_FILENAME} ABSOLUTE)

    if(TARGET Qt6::Svg)
        set(QtSvgTarget Qt6::Svg)
    elseif(TARGET Qt5::Svg)
        set(QtSvgTarget Qt5::Svg)
    endif()
    
    # find svg2qvg target location
    get_target_property(Svg2QvgLocation Qsk::Svg2Qvg LOCATION)
    get_filename_component(Svg2QvgDirectory ${Svg2QvgLocation} DIRECTORY)

    # find svg2qvg target location
    get_target_property(QtSvgTargetLocation ${QtSvgTarget} LOCATION)
    get_filename_component(QtSvgTargetDirectory ${QtSvgTargetLocation} DIRECTORY)

    # construct a platform specific command
    set(cmd "${Svg2QvgBinary} ${SVG_FILENAME} ${QVG_FILENAME}")
    if (CMAKE_SYSTEM_NAME MATCHES "Windows")
        set(cmd "set PATH=\%PATH\%;${QtSvgTargetDirectory} && ${cmd}")
    elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
        set(cmd "DYLD_LIBRARY_PATH=\$DYLD_LIBRARY_PATH:${QtSvgTargetDirectory} ${cmd}")
    elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
        set(cmd "LD_LIBRARY_PATH=\$LD_LIBRARY_PATH:${QtSvgTargetDirectory} ${cmd}")        
    else()
        message(FATAL "Unsupported operating system")
    endif()
    
    add_custom_command(
        COMMAND ${cmd}        
        OUTPUT ${QVG_FILENAME}
        DEPENDS ${SVG_FILENAME}
        COMMENT "Compiling ${SVG_FILENAME} to ${QVG_FILENAME}")
endfunction()

