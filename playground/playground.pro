include( $${PWD}/../qskconfig.pri )

TEMPLATE = subdirs

# qml
SUBDIRS += \
    invoker \
    inputpanel \
    images

qtHaveModule(webengine) {

    SUBDIRS += \
        webview
}
