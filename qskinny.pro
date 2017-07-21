include( qskconfig.pri )

TEMPLATE = subdirs

SUBDIRS = \
    src \
    inputcontext \
    tools \
    support \
    examples

OTHER_FILES = \
    doc/Doxyfile \
    doc/*.dox \
    doc/*.metadox \
    doc/classes/*.dox \
    doc/classes/*.metadox \
    TODO

inputcontext.depends = src
tools.depends = src
support.depends = src
examples.depends = tools support
