/******************************************************************************
 * QSkinny - Copyright (C) 2021 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskArcMetrics.h"

#include <qhashfunctions.h>
#include <qvariant.h>

static void qskRegisterArcMetrics()
{
    qRegisterMetaType< QskArcMetrics >();
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterArcMetrics )

// copied from QskMargins.cpp, we should unify this somehwere:
static inline qreal qskInterpolated( qreal from, qreal to, qreal ratio )
{
    return from + ( to - from ) * ratio;
}

// copied from QskBoxBorderMetrics.cpp, we should unify this somewhere:
static inline qreal qskAbsoluted( qreal length, qreal percentage )
{
    // 100% means -> 0.5 of length
    percentage = qBound( 0.0, percentage, 100.0 );
    return percentage / 100.0 * 0.5 * length;
}

void QskArcMetrics::setWidth( qreal width ) noexcept
{
    m_width = width;
}

void QskArcMetrics::setStartAngle( int startAngle ) noexcept
{
    m_startAngle = startAngle;
}

void QskArcMetrics::setSpanAngle( int spanAngle ) noexcept
{
    m_spanAngle = spanAngle;
}

void QskArcMetrics::setSizeMode( Qt::SizeMode sizeMode ) noexcept
{
    m_sizeMode = sizeMode;
}

QskArcMetrics QskArcMetrics::interpolated(
    const QskArcMetrics& to, qreal ratio ) const noexcept
{
    if ( ( *this == to ) || ( m_sizeMode != to.m_sizeMode ) )
        return to;

    const qreal width = qskInterpolated( m_width, to.m_width, ratio );
    return QskArcMetrics( width, m_startAngle, m_spanAngle, m_sizeMode );
}

QVariant QskArcMetrics::interpolate(
    const QskArcMetrics& from, const QskArcMetrics& to,
    qreal progress )
{
    return QVariant::fromValue( from.interpolated( to, progress ) );
}

QskArcMetrics QskArcMetrics::toAbsolute( const QSizeF& size ) const noexcept
{
    if ( m_sizeMode != Qt::RelativeSize )
        return *this;

    QskArcMetrics absoluted = *this;

    auto& w = absoluted.m_width;

    if ( size.isEmpty() )
    {
        w = 0;
    }
    else
    {
        // for now we just use the width:
        w = qskAbsoluted( size.width(), w );
    }

    absoluted.m_sizeMode = Qt::AbsoluteSize;

    return absoluted;
}

uint QskArcMetrics::hash( uint seed ) const noexcept
{
    uint hash = qHash( m_width, seed );
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

    debug << "Arc" << '(';
    debug << "width:" << metrics.width();
    debug << ", start angle:" << metrics.startAngle();
    debug << ", span angle:" << metrics.spanAngle();
    debug << ", size mode:" << metrics.sizeMode();
    debug << ')';

    return debug;
}

#endif

#include "moc_QskArcMetrics.cpp"
