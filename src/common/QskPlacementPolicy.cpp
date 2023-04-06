/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskPlacementPolicy.h"

static void qskRegisterPlacementPolicy()
{
    qRegisterMetaType< QskPlacementPolicy >();

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    QMetaType::registerEqualsComparator< QskPlacementPolicy >();
#endif
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterPlacementPolicy )

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
