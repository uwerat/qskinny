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
    tabview

lessThan(QT_MAJOR_VERSION, 6): SUBDIRS += iot-dashboard

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
