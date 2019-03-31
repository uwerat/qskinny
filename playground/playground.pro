TEMPLATE = subdirs

SUBDIRS += \
    dialogbuttons \
    invoker \
    inputpanel \
    images

qtHaveModule(webengine) {

    SUBDIRS += \
        webview
}
