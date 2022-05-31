TEMPLATE = subdirs

# c++
SUBDIRS += \
    desktop \
    gallery \
    layouts \
    listbox \
    messagebox \
    mycontrols \
    thumbnails \
    tabview \
    iotdashboard

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
    frames \
    gbenchmark \
    glabels \
    messageboxQml
