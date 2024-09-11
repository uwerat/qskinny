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
    message(STATUS "Svg2QvgLocation: ${Svg2QvgLocation}")
    message(STATUS "Svg2QvgDirectory: ${Svg2QvgDirectory}")

    # find qt svg target location
    get_target_property(QtSvgTargetLocation ${QtSvgTarget} LOCATION)
    get_filename_component(QtSvgTargetDirectory ${QtSvgTargetLocation} DIRECTORY)
    message(STATUS "QtSvgTargetLocation: ${QtSvgTargetLocation}")
    message(STATUS "QtSvgTargetDirectory: ${QtSvgTargetDirectory}")

    # select platform specific wrapper script
    if (CMAKE_SYSTEM_NAME MATCHES "Windows")
        set(script ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/scripts/QSkinnySvg2Qvg.win.bat)
    elseif (CMAKE_SYSTEM_NAME MATCHES "Darwin")
        set(script ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/scripts/QSkinnySvg2Qvg.mac.sh)
    elseif (CMAKE_SYSTEM_NAME MATCHES "Linux")
        set(script ${CMAKE_CURRENT_FUNCTION_LIST_DIR}/scripts/QSkinnySvg2Qvg.lin.sh)
    else()
        message(FATAL "Unsupported operating system")
    endif()
    
    add_custom_command(
        COMMAND ${script} ${Svg2QvgLocation} ${SVG_FILENAME} ${QVG_FILENAME} ${QtSvgTargetDirectory}
        OUTPUT ${QVG_FILENAME}
        DEPENDS ${SVG_FILENAME}
        COMMENT "Compiling ${SVG_FILENAME} to ${QVG_FILENAME}"        
        VERBATIM)
endfunction()

