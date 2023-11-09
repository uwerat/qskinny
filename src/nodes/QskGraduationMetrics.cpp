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
    if ( ( *this == to ) ) // TODO QskGraduationMetrics::sizeMode
    {
        return to;
    }

    return { qskInterpolated( m_minorRatio, to.m_minorRatio, ratio ),
        qskInterpolated( m_mediumRatio, to.m_mediumRatio, ratio ),
        qskInterpolated( m_majorRatio, to.m_majorRatio, ratio ) };
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
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "QskGraduationMetrics";
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
