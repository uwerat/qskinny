TEMPLATE = subdirs

SUBDIRS += \
    anchors \
    dashboard \
    dialogbuttons \
    invoker \
    inputpanel \
    images

SUBDIRS += shadows

qtHaveModule(webengine) {

    SUBDIRS += \
        webview
}

qtHaveModule(quickwidgets) {

    SUBDIRS += \
        grids
}
