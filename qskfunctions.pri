############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
# This file may be used under the terms of the QSkinny License, Version 1.0
############################################################################

# Copied and modified from qt_functions.prf

defineReplace(qskLibraryTarget) {

    unset(LIBRARY_NAME)
    LIBRARY_NAME = $$1

    contains(TEMPLATE, .*lib):CONFIG(debug, debug|release) {

        !debug_and_release|build_pass {

            mac:RET = $$member(LIBRARY_NAME, 0)_debug
            win32:RET = $$member(LIBRARY_NAME, 0)d
        }
    }

    isEmpty(RET):RET = $$LIBRARY_NAME
    return($$RET)
}

defineTest(qskAddLibrary) {

    LIB_PATH = $$1
    LIB_NAME = $$2

    LIBS *= -L$${LIB_PATH}

    unset(LINKAGE)

    isEmpty(LINKAGE) {

        if(!debug_and_release|build_pass):CONFIG(debug, debug|release) {

            mac:LINKAGE = -l$${LIB_NAME}_debug
            win32:LINKAGE = -l$${LIB_NAME}d
        }
    }

    isEmpty(LINKAGE) {

        LINKAGE = -l$${LIB_NAME}
    }

    !isEmpty(QMAKE_LSB) {

        QMAKE_LFLAGS *= --lsb-shared-libs=$${LIB_NAME}
    }

    LIBS += $$LINKAGE
    export(LIBS)
    export(QMAKE_LFLAGS)
    export(QMAKE_LIBDIR_FLAGS)

    return(true)
}
