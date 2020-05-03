/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxShapeMetrics.h"

#include <qhashfunctions.h>
#include <qvariant.h>

#include <algorithm>

static void qskRegisterBoxShapeMetrics()
{
    qRegisterMetaType< QskBoxShapeMetrics >();
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterBoxShapeMetrics )

static inline QSizeF qskInterpolatedSize(
    const QSizeF& from, const QSizeF& to, qreal ratio )
{
    return from + ( to - from ) * ratio;
}

static inline qreal qskAbsoluted( qreal length, qreal percentage )
{
    // 100% means -> 0.5 of length
    percentage = qBound( 0.0, percentage, 100.0 );
    return percentage / 100.0 * 0.5 * length;
}

static inline void qskSetRadius( qreal rx, qreal ry, QSizeF& radius )
{
    radius.rwidth() = ( rx > 0.0 ) ? rx : 0.0;
    radius.rheight() = ( ry > 0.0 ) ? ry : 0.0;
}

void QskBoxShapeMetrics::setRadius(
    qreal topLeftX, qreal topLeftY,
    qreal topRightX, qreal topRightY,
    qreal bottomLeftX, qreal bottomLeftY,
    qreal bottomRightX, qreal bottomRightY ) noexcept
{
    qskSetRadius( topLeftX, topLeftY, m_radii[ Qt::TopLeftCorner ] );
    qskSetRadius( topRightX, topRightY, m_radii[ Qt::TopRightCorner ] );
    qskSetRadius( bottomLeftX, bottomLeftY, m_radii[ Qt::BottomLeftCorner ] );
    qskSetRadius( bottomRightX, bottomRightY, m_radii[ Qt::BottomRightCorner ] );
}

void QskBoxShapeMetrics::setRadius( Qt::Corner corner, qreal radiusX, qreal radiusY ) noexcept
{
    if ( ( corner >= Qt::TopLeftCorner ) && ( corner <= Qt::BottomRightCorner ) )
        qskSetRadius( radiusX, radiusY, m_radii[ corner ] );
}

QskBoxShapeMetrics QskBoxShapeMetrics::transposed() const noexcept
{
    QskBoxShapeMetrics other;

    for ( int i = 0; i < 4; i++ )
        other.m_radii[ i ] = m_radii[ i ].transposed();

    other.m_sizeMode = m_sizeMode;

    return other;
}

QskBoxShapeMetrics QskBoxShapeMetrics::toAbsolute( const QSizeF& size ) const noexcept
{
    if ( m_sizeMode != Qt::RelativeSize )
        return *this;

    QskBoxShapeMetrics absoluted = *this;

    if ( size.isEmpty() )
    {
        for ( int i = 0; i < 4; i++ )
            absoluted.m_radii[ i ] = QSizeF( 0.0, 0.0 );
    }
    else
    {
        for ( int i = 0; i < 4; i++ )
        {
            auto& radii = absoluted.m_radii[ i ];

            const qreal rx = qskAbsoluted( size.width(), radii.width() );
            const qreal ry = qskAbsoluted( size.height(), radii.height() );

            switch ( m_aspectRatioMode )
            {
                case Qt::IgnoreAspectRatio:
                {
                    radii.setWidth( rx );
                    radii.setHeight( ry );
                    break;
                }
                case Qt::KeepAspectRatio:
                {
                    radii.setWidth( std::min( rx, ry ) );
                    radii.setHeight( std::min( rx, ry ) );
                    break;
                }
                case Qt::KeepAspectRatioByExpanding:
                {
                    radii.setWidth( std::max( rx, ry ) );
                    radii.setHeight( std::max( rx, ry ) );
                    break;
                }
            }
        }
    }
    absoluted.m_sizeMode = Qt::AbsoluteSize;

    return absoluted;
}

QskBoxShapeMetrics QskBoxShapeMetrics::interpolated(
    const QskBoxShapeMetrics& to, qreal ratio ) const noexcept
{
    if ( ( *this == to ) || ( m_sizeMode != to.m_sizeMode ) )
        return to;

    QSizeF radii[ 4 ];
    radii[ 0 ] = qskInterpolatedSize( m_radii[ 0 ], to.m_radii[ 0 ], ratio );
    radii[ 1 ] = qskInterpolatedSize( m_radii[ 1 ], to.m_radii[ 1 ], ratio );
    radii[ 2 ] = qskInterpolatedSize( m_radii[ 2 ], to.m_radii[ 2 ], ratio );
    radii[ 3 ] = qskInterpolatedSize( m_radii[ 3 ], to.m_radii[ 3 ], ratio );

    return QskBoxShapeMetrics( to.m_sizeMode, radii );
}

QVariant QskBoxShapeMetrics::interpolate(
    const QskBoxShapeMetrics& from, const QskBoxShapeMetrics& to, qreal progress ) noexcept
{
    return QVariant::fromValue( from.interpolated( to, progress ) );
}

uint QskBoxShapeMetrics::hash( uint seed ) const noexcept
{
    uint hash = qHashBits( m_radii, sizeof( m_radii ), seed );

    const int mode = m_sizeMode;
    return qHashBits( &mode, sizeof( mode ), hash );
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskBoxShapeMetrics& metrics )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "BoxShape" << '(';
    debug << metrics.sizeMode();

    for ( int i = Qt::TopLeftCorner; i <= Qt::BottomRightCorner; i++ )
    {
        const QSizeF r = metrics.radius( static_cast< Qt::Corner >( i ) );
        debug << "(" << r.width() << ',' << r.height() << ")";
    }

    debug << ')';

    return debug;
}

#endif
