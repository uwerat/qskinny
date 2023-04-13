/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskArcMetrics.h"
#include "QskFunctions.h"

#include <qhashfunctions.h>
#include <qvariant.h>

static void qskRegisterArcMetrics()
{
    qRegisterMetaType< QskArcMetrics >();

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    QMetaType::registerEqualsComparator< QskArcMetrics >();
#endif
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterArcMetrics )

static inline qreal qskInterpolated( qreal from, qreal to, qreal ratio )
{
    return from + ( to - from ) * ratio;
}

static inline qreal qskAbsoluted( qreal length, qreal percentage )
{
    // 100% means -> 0.5 of length
    percentage = qBound( 0.0, percentage, 100.0 );
    return percentage / 100.0 * 0.5 * length;
}

void QskArcMetrics::setThickness( qreal thickness ) noexcept
{
    m_thickness = thickness;
}

void QskArcMetrics::setStartAngle( qreal startAngle ) noexcept
{
    m_startAngle = qskConstrainedDegrees( startAngle );
}

void QskArcMetrics::setSpanAngle( qreal spanAngle ) noexcept
{
    m_spanAngle = qBound( -360.0, spanAngle, 360.0 );
}

void QskArcMetrics::setSizeMode( Qt::SizeMode sizeMode ) noexcept
{
    m_sizeMode = sizeMode;
}

bool QskArcMetrics::containsAngle( qreal angle ) const
{
    angle = qskConstrainedDegrees( angle );

    if ( m_spanAngle >= 0.0 )
    {
        if ( angle < m_startAngle )
            angle += 360.0;

        return ( angle >= m_startAngle ) && ( angle <= m_startAngle + m_spanAngle );
    }
    else
    {
        if ( angle > m_startAngle )
            angle -= 360.0;

        return ( angle <= m_startAngle ) && ( angle >= m_startAngle + m_spanAngle );
    }
}

QskArcMetrics QskArcMetrics::interpolated(
    const QskArcMetrics& to, qreal ratio ) const noexcept
{
    if ( ( *this == to ) || ( m_sizeMode != to.m_sizeMode ) )
        return to;

    const qreal thickness = qskInterpolated( m_thickness, to.m_thickness, ratio );

    const qreal s1 = qskInterpolated( m_startAngle, to.m_startAngle, ratio );
    const qreal s2 = qskInterpolated( endAngle(), to.endAngle(), ratio );

    return QskArcMetrics( s1, s2 - s1, thickness, m_sizeMode );
}

QVariant QskArcMetrics::interpolate(
    const QskArcMetrics& from, const QskArcMetrics& to,
    qreal progress )
{
    return QVariant::fromValue( from.interpolated( to, progress ) );
}

QskArcMetrics QskArcMetrics::toAbsolute( const QSizeF& size ) const noexcept
{
    if ( size.width() < 0.0 )
        return toAbsolute( size.height() );

    if ( size.height() < 0.0 )
        return toAbsolute( size.width() );

    return toAbsolute( qMin( size.width(), size.height() ) );
}

QskArcMetrics QskArcMetrics::toAbsolute( qreal size ) const noexcept
{
    if ( m_sizeMode != Qt::RelativeSize )
        return *this;

    QskArcMetrics absoluted = *this;

    if ( size <= 0.0 )
        absoluted.m_thickness = 0.0;
    else
        absoluted.m_thickness = qskAbsoluted( size, absoluted.m_thickness );

    absoluted.m_sizeMode = Qt::AbsoluteSize;

    return absoluted;
}

QskHashValue QskArcMetrics::hash( QskHashValue seed ) const noexcept
{
    auto hash = qHash( m_thickness, seed );
    hash = qHash( m_startAngle, hash );
    hash = qHash( m_spanAngle, hash );

    const int mode = m_sizeMode;
    return qHashBits( &mode, sizeof( mode ), hash );
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskArcMetrics& metrics )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "QskArcMetrics" << '(';
    debug << metrics.thickness() << ',' << metrics.sizeMode();
    debug << ",[" << metrics.startAngle() << ',' << metrics.spanAngle() << ']';
    debug << ')';

    return debug;
}

#endif

#include "moc_QskArcMetrics.cpp"
