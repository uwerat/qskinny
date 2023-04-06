/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
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
