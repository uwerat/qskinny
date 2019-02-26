include( $${PWD}/../qskconfig.pri )

TEMPLATE = subdirs

# c++
SUBDIRS += \
    desktop \
    layouts \
    listbox \
    messagebox \
    mycontrols \
    sliders \
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
    messageboxQml \
    tlabels
