TEMPLATE = subdirs

qtHaveModule(svg):!cross_compile {

    SUBDIRS += \
        svg2qvg
}

doxygen {
    SUBDIRS += \
        metadoxfilter
}
