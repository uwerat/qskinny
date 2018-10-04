include( qskconfig.pri )

TEMPLATE = subdirs

SUBDIRS = \
    src \
    skins \
    inputcontext \
    qmlexport \
    tools \
    support \
    examples \
    playground

OTHER_FILES = \
    doc/Doxyfile \
    doc/*.dox \
    doc/*.metadox \
    doc/classes/*.dox \
    doc/classes/*.metadox \
    TODO

inputcontext.depends = src
skins.depends = src
tools.depends = src
support.depends = skins
examples.depends = tools support skins qmlexport
playground.depends = tools support skins qmlexport
