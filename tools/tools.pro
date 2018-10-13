include( $${PWD}/../qskconfig.pri )

TEMPLATE = subdirs

qtHaveModule(svg) {

    SUBDIRS += \
        svg2qvg
}

doxygen {
    SUBDIRS += \
        metadoxfilter
}
