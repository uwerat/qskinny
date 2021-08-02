TEMPLATE = subdirs

# c++
SUBDIRS += \
    desktop \
    switchbuttons \
    gallery \
    layouts \
    listbox \
    messagebox \
    mycontrols \
    thumbnails \
    tabview

lessThan(QT_MAJOR_VERSION, 6) {

    # the shader for the drop shadows has not yet been migrated
    # to work with Qt 6

    SUBDIRS += iotdashboard
}

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
