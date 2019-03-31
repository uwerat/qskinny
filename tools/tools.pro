TEMPLATE = subdirs

qtHaveModule(svg) {

    SUBDIRS += \
        svg2qvg
}

doxygen {
    SUBDIRS += \
        metadoxfilter
}
