include( $${PWD}/../qskconfig.pri )

TEMPLATE = subdirs

SUBDIRS += \
    svg2qvg \

doxygen {
    SUBDIRS += \
        metadoxfilter
}
