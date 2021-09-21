TEMPLATE = subdirs

SUBDIRS += \
    anchors \
    dialogbuttons \
    invoker \
    inputpanel \
    images

qtHaveModule(webengine) {

    SUBDIRS += \
        webview
}

qtHaveModule(quickwidgets) {

    SUBDIRS += \
        grids
}
