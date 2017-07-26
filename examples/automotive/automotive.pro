include( $${PWD}/../examples.pri )

TARGET = automotive

HEADERS += \
    ButtonBar.h \
    RadioControl.h \
    SoundControl.h \
    SkinFactory.h \
    DefaultSkin.h \
    OtherSkin.h \
    MainWindow.h

SOURCES += \
    ButtonBar.cpp \
    RadioControl.cpp \
    SoundControl.cpp \
    SkinFactory.cpp \
    DefaultSkin.cpp \
    OtherSkin.cpp \
    MainWindow.cpp \
    main.cpp

QRCFILES += \
    images.qrc

SVGSOURCES = \
    images/car.svg

SVGSOURCES += \
    images/left.svg \
    images/down.svg \
    images/right.svg \
    images/up.svg

SVGSOURCES += \
    images/bluetooth.svg \
    images/bookmark.svg \
    images/bus.svg \
    images/cloud.svg \
    images/compass.svg \
    images/location.svg \
    images/man.svg \
    images/menu.svg \
    images/phone.svg \
    images/plane.svg \
    images/train.svg \
    images/user.svg \


###########
# The rcc file includes the precompiled SVGs and so we need to build
# rules for running svg2qvg, when a SVG has changed into the Makefile
# before running rccgen.
# While this would be an easy one in Makefile syntax it is a nightmare
# with qmake.

# TODO: we should at least offer some sort of *.pri file to help with
# writing application project files.
###########

SVG2QVG=$${QSK_OUT_ROOT}/tools/bin/svg2qvg

svg2qvg.name = SVG compiler
svg2qvg.input = SVGSOURCES
svg2qvg.output = qvg/${QMAKE_FILE_BASE}.qvg
svg2qvg.variable_out =
svg2qvg.commands += mkdir -p qvg && $${SVG2QVG} ${QMAKE_FILE_IN} $${svg2qvg.output}

rccgen.name = RCC compiler
rccgen.input = QRCFILES
rccgen.output = $${RCC_DIR}/qrc_${QMAKE_FILE_BASE}.cpp
rccgen.variable_out = SOURCES

rccgen.commands += $${QMAKE_MKDIR} $${RCC_DIR}

!equals( OUT_PWD, $${PWD} ) {

    # Paths inside a qrc file are always relative to the path of the
    # qrc file itself. So in case of shadow builds we need to copy the
    # qrc file into the shadow directory as the included qvg files
    # are generated locally.

    QRC_SHADOW_CLONE = $${OUT_PWD}/${QMAKE_FILE_BASE}_shadow.qrc

    rccgen.commands += && $${QMAKE_COPY} ${QMAKE_FILE_IN} $${QRC_SHADOW_CLONE}
    rccgen.commands += && $$dirname(QMAKE_QMAKE)/rcc $${QRC_SHADOW_CLONE} -o ${QMAKE_FILE_OUT}
    rccgen.commands += && $${QMAKE_DEL_FILE} $${QRC_SHADOW_CLONE}
}
else {
    rccgen.commands += && $$dirname(QMAKE_QMAKE)/rcc ${QMAKE_FILE_IN} -o ${QMAKE_FILE_OUT}
}

# We would like to define a dependency for "svg2qvg.variable_out" -
# but how to do this ? As long as we don't have a solution we have to use
# the workaround below.

defineReplace(qvgfiles) {
    svgfiles = $$1

    myfiles=
    for(svgfile, svgfiles) myfiles += $$replace(svgfile, svg, qvg)

    myfiles2=
    for(myfile, myfiles) myfiles2 += $$replace(myfile, images, qvg)

    return($$myfiles2)
}
rccgen.depends += $$qvgfiles( $${SVGSOURCES} )

QMAKE_EXTRA_COMPILERS += svg2qvg rccgen
