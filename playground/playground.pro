TEMPLATE = subdirs

SUBDIRS += \
    anchors \
    dialogbuttons \
    invoker \
    inputpanel \
    images

lessThan(QT_MAJOR_VERSION, 6) {

    # the shader for the drop shadows has not yet been migrated
    # to work with Qt 6

    SUBDIRS += shadows
}

qtHaveModule(webengine) {

    SUBDIRS += \
        webview
}

qtHaveModule(quickwidgets) {

    SUBDIRS += \
        grids
}
