/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskAnimationHint.h"

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskAnimationHint& hint )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "AnimationHint" << '(';
    debug << hint.duration << ',' << hint.type << ',' << hint.updateFlags;
    debug << ')';

    return debug;
}

#endif
