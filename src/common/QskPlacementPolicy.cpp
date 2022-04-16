/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskPlacementPolicy.h"

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskPlacementPolicy policy )
{
    QDebugStateSaver saver( debug );
    debug.nospace();
    debug << "PlacementPolicy" << '(';
    debug << policy.visiblePolicy() << ", " << policy.hiddenPolicy();
    debug << ')';

    return debug;
}

#endif

#include "moc_QskPlacementPolicy.cpp"
