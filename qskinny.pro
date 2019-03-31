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

qmlexport.depends = src
inputcontext.depends = src
skins.depends = src
support.depends = src skins
examples.depends = tools support skins qmlexport
playground.depends = tools support skins qmlexport
