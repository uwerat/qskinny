#include "QskGraduationMetrics.h"

#include <qvariant.h>

static void qskRegisterGraduationMetrics()
{
    qRegisterMetaType< QskGraduationMetrics >();

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    QMetaType::registerEqualsComparator< QskGraduationMetrics >();
#endif
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterGraduationMetrics )

static inline qreal qskInterpolated( qreal from, qreal to, qreal ratio )
{
    return from + ( to - from ) * ratio;
}

QskGraduationMetrics QskGraduationMetrics::interpolated(
    const QskGraduationMetrics& to, const qreal ratio ) const noexcept
{
    if ( ( *this == to ) )
        return to;

    return { qskInterpolated( m_tickLengths[0], to.m_tickLengths[0], ratio ),
        qskInterpolated( m_tickLengths[1], to.m_tickLengths[1], ratio ),
        qskInterpolated( m_tickLengths[2], to.m_tickLengths[2], ratio ) };
}

QVariant QskGraduationMetrics::interpolate(
    const QskGraduationMetrics& from, const QskGraduationMetrics& to, const qreal progress )
{
    return QVariant::fromValue( from.interpolated( to, progress ) );
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskGraduationMetrics& metrics )
{
    const char s[] = ", ";

    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "Graduation";
    debug << '(';
    debug << metrics.minorTickLength() << s << metrics.mediumTickLength()
        << s << metrics.majorTickLength();
    debug << ')';

    return debug;
}

#endif
