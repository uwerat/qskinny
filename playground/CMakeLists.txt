add_subdirectory(anchors)
add_subdirectory(dials)
add_subdirectory(dialogbuttons)
add_subdirectory(invoker)
add_subdirectory(inputpanel)
add_subdirectory(images)
add_subdirectory(shadows)

# if(TARGET Qt5::WebEngine) # TODO check how to add optionally
if(TRUE)
    add_subdirectory(webview)
endif()

# if(TARGET Qt5::QuickWidgets) # TODO check how to add optionally
if(TRUE)
    add_subdirectory(grids)
endif()
