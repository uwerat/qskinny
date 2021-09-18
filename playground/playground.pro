TEMPLATE = subdirs

SUBDIRS += \
    anchors \
    dialogbuttons \
    invoker \
    inputpanel \
    shadows \
    images

qtHaveModule(webengine) {

    SUBDIRS += \
        webview
}

qtHaveModule(quickwidgets) {

    SUBDIRS += \
        grids
}
