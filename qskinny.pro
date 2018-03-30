include( qskconfig.pri )

TEMPLATE = subdirs

SUBDIRS = \
    src \
    skins \
    inputcontext \
    tools \
    support \
    examples \
    playground \
    3rdparty

OTHER_FILES = \
    doc/Doxyfile \
    doc/*.dox \
    doc/*.metadox \
    doc/classes/*.dox \
    doc/classes/*.metadox \
    TODO

inputcontext.depends = src 3rdparty
skins.depends = src
tools.depends = src
support.depends = skins
examples.depends = tools support skins
playground.depends = tools support skins
