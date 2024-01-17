/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "ChartSample.h"

static void qskRegisterChartSample()
{
    qRegisterMetaType< ChartSample >();

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    QMetaType::registerEqualsComparator< ChartSample >();
#endif
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterChartSample )

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const ChartSample& sample )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "ChartSample" << "( " << sample.title() << ": "
        << sample.value() << sample.gradient() << " )";
    return debug;
}

#endif

#include "moc_ChartSample.cpp"
