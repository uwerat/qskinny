############################################################################
# QSkinny - Copyright (C) 2016 Uwe Rathmann
# This file may be used under the terms of the QSkinny License, Version 1.0
############################################################################

defineReplace(qskLibraryTarget) {

    return( $$qtLibraryTarget($$1) )
}

defineReplace(qskPluginTarget) {

    return( $$qtLibraryTarget($$1) )
}

defineTest(qskAddLibrary) {

    LIBS *= -L$$1
    LIBS += -l$$2$$qtPlatformTargetSuffix()

    export(LIBS)

    return(true)
}
