TEMPLATE = subdirs

# c++
SUBDIRS += \
    desktop \
    gallery \
    iot-dashboard \
    layouts \
    listbox \
    messagebox \
    mycontrols \
    thumbnails \
    tabview

qtHaveModule(svg) {

    # when checking in qvg files we could drop the svg dependency 

    SUBDIRS += \
        automotive \
        qvgviewer
}

# qml
SUBDIRS += \
    boxes \
    buttons \
    colorswitch \
    frames \
    gbenchmark \
    glabels \
    messageboxQml
