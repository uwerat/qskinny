/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskArcBorderMetrics.h"

#include <qhashfunctions.h>
#include <qvariant.h>

static void qskRegisterArcBorderMetrics()
{
    qRegisterMetaType< QskArcBorderMetrics >();

    QMetaType::registerConverter< int, QskArcBorderMetrics >(
        []( int width ) { return QskArcBorderMetrics( width ); } );

    QMetaType::registerConverter< qreal, QskArcBorderMetrics >(
        []( qreal width ) { return QskArcBorderMetrics( width ); } );
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterArcBorderMetrics )

// copied from QskMargins.cpp, we should unify this somehwere:
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

void QskArcBorderMetrics::setSizeMode( Qt::SizeMode sizeMode ) noexcept
{
    m_sizeMode = sizeMode;
}

void QskArcBorderMetrics::setWidth( Qsk::ArcPosition position, qreal width ) noexcept
{
    m_widths[ position ] =  qMax( width, 0.0 );
}

void QskArcBorderMetrics::setWidths( qreal width ) noexcept
{
    m_widths[0] = m_widths[1] = m_widths[2] = m_widths[3] = width;
}

void QskArcBorderMetrics::setWidths( qreal outer, qreal inner, qreal start,
    qreal end ) noexcept
{
    m_widths[0] = outer;
    m_widths[1] = inner;
    m_widths[2] = start;
    m_widths[3] = end;
}

void QskArcBorderMetrics::setOuterWidth( qreal width ) noexcept
{
    m_widths[0] = width;
}

void QskArcBorderMetrics::setInnerWidth( qreal width ) noexcept
{
    m_widths[1] = width;
}

void QskArcBorderMetrics::setStartWidth( qreal width ) noexcept
{
    m_widths[2] = width;
}

void QskArcBorderMetrics::setEndWidth( qreal width ) noexcept
{
    m_widths[3] = width;
}

QskArcBorderMetrics QskArcBorderMetrics::interpolated(
    const QskArcBorderMetrics& to, qreal ratio ) const noexcept
{
    if ( ( *this == to ) || ( m_sizeMode != to.m_sizeMode ) )
        return to;

    const qreal outerWidth = qskInterpolated( m_widths[0], to.m_widths[0], ratio );
    const qreal innerWidth = qskInterpolated( m_widths[1], to.m_widths[1], ratio );
    const qreal startWidth = qskInterpolated( m_widths[2], to.m_widths[2], ratio );
    const qreal endWidth = qskInterpolated( m_widths[3], to.m_widths[3], ratio );

    return QskArcBorderMetrics( outerWidth, innerWidth, startWidth, endWidth,
        m_sizeMode );
}

QVariant QskArcBorderMetrics::interpolate(
    const QskArcBorderMetrics& from, const QskArcBorderMetrics& to,
    qreal progress )
{
    return QVariant::fromValue( from.interpolated( to, progress ) );
}

QskArcBorderMetrics QskArcBorderMetrics::toAbsolute( const QSizeF& size ) const noexcept
{
    if ( m_sizeMode != Qt::RelativeSize )
        return *this;

    QskArcBorderMetrics absoluted = *this;

    auto& outerWidth = absoluted.m_widths[0];
    auto& innerWidth = absoluted.m_widths[1];
    auto& startWidth = absoluted.m_widths[2];
    auto& endWidth = absoluted.m_widths[3];

    if ( size.isEmpty() )
    {
        outerWidth = 0.0;
        innerWidth = 0.0;
        startWidth = 0.0;
        endWidth = 0.0;
    }
    else
    {
        // for now we just use the width:
        outerWidth = qskAbsoluted( size.width(), outerWidth );
        innerWidth = qskAbsoluted( size.width(), innerWidth );
        startWidth = qskAbsoluted( size.width(), startWidth );
        endWidth = qskAbsoluted( size.width(), endWidth );
    }

    absoluted.m_sizeMode = Qt::AbsoluteSize;

    return absoluted;
}

uint QskArcBorderMetrics::hash( uint seed ) const noexcept
{
    uint hash = qHash( m_widths[0], seed );
    hash = qHash( m_widths[1], hash );
    hash = qHash( m_widths[2], hash );
    hash = qHash( m_widths[3], hash );

    const int mode = m_sizeMode;
    return qHashBits( &mode, sizeof( mode ), hash );
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskArcBorderMetrics& metrics )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "ArcBorder" << '(';
    debug << metrics.sizeMode();
    debug << ", outer width:" << metrics.width( Qsk::Outer );
    debug << ", inner width:" << metrics.width( Qsk::Inner );
    debug << ", start width:" << metrics.width( Qsk::Start );
    debug << ", end width:" << metrics.width( Qsk::End );
    debug << ')';

    return debug;
}

#endif

#include "moc_QskArcBorderMetrics.cpp"
