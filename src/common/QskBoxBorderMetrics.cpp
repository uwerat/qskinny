/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxBorderMetrics.h"

#include <qhashfunctions.h>
#include <qvariant.h>

static void qskRegisterBoxBorderMetrics()
{
    qRegisterMetaType< QskBoxBorderMetrics >();

    QMetaType::registerConverter< QskMargins, QskBoxBorderMetrics >(
        []( const QskMargins& margins ) { return QskBoxBorderMetrics( margins ); } );

    QMetaType::registerConverter< int, QskBoxBorderMetrics >(
        []( int width ) { return QskBoxBorderMetrics( width ); } );

    QMetaType::registerConverter< qreal, QskBoxBorderMetrics >(
        []( qreal width ) { return QskBoxBorderMetrics( width ); } );
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterBoxBorderMetrics )

static inline qreal qskAbsoluted( qreal length, qreal percentage )
{
    // 100% means -> 0.5 of length
    percentage = qBound( 0.0, percentage, 100.0 );
    return percentage / 100.0 * 0.5 * length;
}

void QskBoxBorderMetrics::setSizeMode( Qt::SizeMode sizeMode ) noexcept
{
    m_sizeMode = sizeMode;
}

void QskBoxBorderMetrics::setWidths( const QskMargins& widths ) noexcept
{
    m_widths.setLeft( qMax( widths.left(), 0.0 ) );
    m_widths.setTop( qMax( widths.top(), 0.0 ) );
    m_widths.setRight( qMax( widths.right(), 0.0 ) );
    m_widths.setBottom( qMax( widths.bottom(), 0.0 ) );
}

void QskBoxBorderMetrics::setWidthAt( Qt::Edges edges, qreal width ) noexcept
{
    m_widths.setMarginsAt( edges, qMax( width, 0.0 ) );
}

QskBoxBorderMetrics QskBoxBorderMetrics::toAbsolute( const QSizeF& size ) const noexcept
{
    if ( m_sizeMode != Qt::RelativeSize )
        return *this;

    QskBoxBorderMetrics absoluted = *this;

    auto& w = absoluted.m_widths;

    if ( size.isEmpty() )
    {
        w = QskMargins();
    }
    else
    {
        w.setLeft( qskAbsoluted( size.width(), w.left() ) );
        w.setTop( qskAbsoluted( size.height(), w.top() ) );
        w.setRight( qskAbsoluted( size.width(), w.right() ) );
        w.setBottom( qskAbsoluted( size.height(), w.bottom() ) );
    }

    absoluted.m_sizeMode = Qt::AbsoluteSize;

    return absoluted;
}

QskBoxBorderMetrics QskBoxBorderMetrics::interpolated(
    const QskBoxBorderMetrics& to, qreal ratio ) const noexcept
{
    if ( ( *this == to ) || ( m_sizeMode != to.m_sizeMode ) )
        return to;

    const auto widths = m_widths.interpolated( to.m_widths, ratio );
    return QskBoxBorderMetrics( widths, m_sizeMode );
}

QVariant QskBoxBorderMetrics::interpolate(
    const QskBoxBorderMetrics& from, const QskBoxBorderMetrics& to,
    qreal progress )
{
    return QVariant::fromValue( from.interpolated( to, progress ) );
}

uint QskBoxBorderMetrics::hash( uint seed ) const noexcept
{
    uint hash = qHashBits( &m_widths, sizeof( m_widths ), seed );

    const int mode = m_sizeMode;
    return qHashBits( &mode, sizeof( mode ), hash );
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskBoxBorderMetrics& metrics )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "BoxBorder" << '(';
    debug << metrics.sizeMode() << ',' << metrics.widths();
    debug << ')';

    return debug;
}

#endif
