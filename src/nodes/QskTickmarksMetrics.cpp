#include "QskTickmarksMetrics.h"

#include <QVariant>

static void qskRegisterTickmarksMetrics()
{
    qRegisterMetaType< QskTickmarksMetrics >();

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    QMetaType::registerEqualsComparator< QskTickmarksMetrics >();
#endif
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterTickmarksMetrics )

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskTickmarksMetrics& metrics )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "QskTickmarksMetrics";
    debug << '(';
    debug << "minor: ";
    debug << metrics.minorRatio();
    debug << ',';
    debug << "medium: ";
    debug << metrics.mediumRatio();
    debug << ',';
    debug << "major: ";
    debug << metrics.majorRatio();
    debug << ')';

    return debug;
}

#endif
