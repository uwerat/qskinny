/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxShapeMetrics.h"

#include <qhashfunctions.h>
#include <QDebug>

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

QskBoxShapeMetrics::QskBoxShapeMetrics(
        qreal radiusX, qreal radiusY, Qt::SizeMode sizeMode ):
    m_sizeMode( sizeMode ),
    m_aspectRatioMode( Qt::KeepAspectRatio )
{
    setRadius( radiusX, radiusY );
}

QskBoxShapeMetrics::~QskBoxShapeMetrics()
{
}

bool QskBoxShapeMetrics::operator==( const QskBoxShapeMetrics& other ) const
{
    if ( m_sizeMode != other.m_sizeMode )
        return false;

    if ( m_aspectRatioMode != other.m_aspectRatioMode )
        return false;

    for ( size_t i = 0; i < 4; i++ )
    {
        if ( m_radii[i] != other.m_radii[i] )
            return false;
    }

    return true;
}

void QskBoxShapeMetrics::setSizeMode( Qt::SizeMode sizeMode )
{
    m_sizeMode = sizeMode;
}

void QskBoxShapeMetrics::setAspectRatioMode( Qt::AspectRatioMode aspectRatioMode )
{
    m_aspectRatioMode = aspectRatioMode;
}

void QskBoxShapeMetrics::setRadius(
    qreal topLeftX, qreal topLeftY, qreal topRightX, qreal topRightY,
    qreal bottomLeftX, qreal bottomLeftY, qreal bottomRightX, qreal bottomRightY )
{
    m_radii[ Qt::TopLeftCorner ].setWidth( qMax( topLeftX, 0.0 ) );
    m_radii[ Qt::TopLeftCorner ].setHeight( qMax( topLeftY, 0.0 ) );

    m_radii[ Qt::TopRightCorner ].setWidth( qMax( topRightX, 0.0 ) );
    m_radii[ Qt::TopRightCorner ].setHeight( qMax( topRightY, 0.0 ) );

    m_radii[ Qt::BottomLeftCorner ].setWidth( qMax( bottomLeftX, 0.0 ) );
    m_radii[ Qt::BottomLeftCorner ].setHeight( qMax( bottomLeftY, 0.0 ) );

    m_radii[ Qt::BottomRightCorner ].setWidth( qMax( bottomRightX, 0.0 ) );
    m_radii[ Qt::BottomRightCorner ].setHeight( qMax( bottomRightY, 0.0 ) );
}

void QskBoxShapeMetrics::setRadius( Qt::Corner corner, qreal radiusX, qreal radiusY )
{
    if ( ( corner >= Qt::TopLeftCorner ) && ( corner <= Qt::BottomRightCorner ) )
    {
        m_radii[ corner ].setWidth( qMax( radiusX, 0.0 ) );
        m_radii[ corner ].setHeight( qMax( radiusY, 0.0 ) );
    }
}

bool QskBoxShapeMetrics::isRectellipse() const
{
    const QSizeF* r = m_radii;
    return ( r[1] == r[0] ) && ( r[2] == r[1] ) && ( r[3] == r[2] );
}

bool QskBoxShapeMetrics::isRectangle() const
{
    const QSizeF* r = m_radii;

    if ( ( r[0].width() > 0.0 ) || ( r[0].height() > 0.0 ) )
        return false;

    if ( ( r[1].width() > 0.0 ) || ( r[1].height() > 0.0 ) )
        return false;

    if ( ( r[2].width() > 0.0 ) || ( r[2].height() > 0.0 ) )
        return false;

    if ( ( r[3].width() > 0.0 ) || ( r[3].height() > 0.0 ) )
        return false;

    return true;
}

QskBoxShapeMetrics QskBoxShapeMetrics::rotated() const
{
    QskBoxShapeMetrics other;

    for ( int i = 0; i < 4; i++ )
        other.m_radii[i] = m_radii[i].transposed();

    other.m_sizeMode = m_sizeMode;

    return other;
}

QskBoxShapeMetrics QskBoxShapeMetrics::toAbsolute( const QSizeF& size ) const
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

            switch( m_aspectRatioMode )
            {
                case Qt::IgnoreAspectRatio:
                {
                    radii.setWidth( rx );
                    radii.setHeight( ry );
                    break;
                }
                case Qt::KeepAspectRatio:
                {
                    radii.setWidth( qMin( rx, ry ) );
                    radii.setHeight( qMin( rx, ry ) );
                    break;
                }
                case Qt::KeepAspectRatioByExpanding:
                {
                    radii.setWidth( qMax( rx, ry ) );
                    radii.setHeight( qMax( rx, ry ) );
                    break;
                }
            }
        }
    }
    absoluted.m_sizeMode = Qt::AbsoluteSize;

    return absoluted;
}

QskBoxShapeMetrics QskBoxShapeMetrics::interpolated(
    const QskBoxShapeMetrics& to, qreal ratio ) const
{
    if ( ( *this == to ) || ( m_sizeMode != to.m_sizeMode ) )
        return to;

    QSizeF radii[4];
    radii[0] = qskInterpolatedSize( m_radii[0], to.m_radii[0], ratio );
    radii[1] = qskInterpolatedSize( m_radii[1], to.m_radii[1], ratio );
    radii[2] = qskInterpolatedSize( m_radii[2], to.m_radii[2], ratio );
    radii[3] = qskInterpolatedSize( m_radii[3], to.m_radii[3], ratio );

    return QskBoxShapeMetrics( to.m_sizeMode, radii );
}

QVariant QskBoxShapeMetrics::interpolate( const QskBoxShapeMetrics& from,
    const QskBoxShapeMetrics& to, qreal progress )
{
    return QVariant::fromValue( from.interpolated( to, progress ) );
}

uint QskBoxShapeMetrics::hash( uint seed ) const
{
    uint hash = qHashBits( m_radii, sizeof( m_radii ), seed );

    const int mode = m_sizeMode;
    return qHashBits( &mode, sizeof( mode ), hash );
}

#ifndef QT_NO_DEBUG_STREAM

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

