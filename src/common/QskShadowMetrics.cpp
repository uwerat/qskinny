/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskShadowMetrics.h"

#include <qhashfunctions.h>
#include <qrect.h>
#include <qvariant.h>

static void qskRegisterShadowMetrics()
{
    qRegisterMetaType< QskShadowMetrics >();
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterShadowMetrics )

static inline qreal qskInterpolated( qreal from, qreal to, qreal ratio )
{
    return from + ( to - from ) * ratio;
}

static inline qreal qskToAbsolute( qreal length, qreal percentage )
{
    percentage = qBound( 0.0, percentage, 100.0 );
    return percentage / 100.0 * length;
}

QskShadowMetrics QskShadowMetrics::toAbsolute( const QSizeF& size ) const noexcept
{
    if ( m_sizeMode != Qt::RelativeSize )
        return *this;

    if ( size.isEmpty() )
        return QskShadowMetrics( 0.0, 0.0, QPointF() );

    const qreal length = std::max( size.width(), size.height() );

    const qreal spreadRadius = qskToAbsolute( length, m_spreadRadius );
    const qreal blurRadius = qskToAbsolute( length, m_spreadRadius );
    const qreal dx = qskToAbsolute( size.width(), m_offset.x() );
    const qreal dy = qskToAbsolute( size.height(), m_offset.x() );

    return QskShadowMetrics( spreadRadius, blurRadius, QPointF( dx, dy ) );
}

QskShadowMetrics QskShadowMetrics::interpolated(
    const QskShadowMetrics& to, qreal ratio ) const noexcept
{
    if ( ( *this == to ) || ( m_sizeMode != to.m_sizeMode ) )
        return to;

    const QPointF offset(
        qskInterpolated( m_offset.x(), to.m_offset.x(), ratio ),
        qskInterpolated( m_offset.y(), to.m_offset.y(), ratio ) );

    QskShadowMetrics metrics(
        qskInterpolated( m_spreadRadius, to.m_spreadRadius, ratio ),
        qskInterpolated( m_blurRadius, to.m_blurRadius, ratio ),
        offset );
    metrics.m_sizeMode = m_sizeMode;

    return metrics;
}

QVariant QskShadowMetrics::interpolate(
    const QskShadowMetrics& from, const QskShadowMetrics& to,
    qreal progress )
{
    return QVariant::fromValue( from.interpolated( to, progress ) );
}

QRectF QskShadowMetrics::shadowRect( const QRectF& sourceRect ) const
{
    return QRectF(
        sourceRect.x() + m_offset.x() - m_spreadRadius,
        sourceRect.y() + m_offset.y() - m_spreadRadius,
        sourceRect.width() + 2 * m_spreadRadius,
        sourceRect.height() + 2 * m_spreadRadius );
}

uint QskShadowMetrics::hash( uint seed ) const noexcept
{
    uint hash;

    hash = qHash( m_offset.x(), seed );
    hash = qHash( m_offset.y(), seed );
    hash = qHash( m_spreadRadius, hash );
    hash = qHash( m_blurRadius, hash );
    hash = qHash( static_cast< int >( m_sizeMode ), hash );

    return hash;
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskShadowMetrics& metrics )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "Shadow" << '(';
    debug << " spread:" << metrics.spreadRadius();
    debug << " blur:" << metrics.blurRadius();
    debug << " dx:" << metrics.offset().x();
    debug << " dy:" << metrics.offset().y();
    debug << " )";

    return debug;
}

#endif
