/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBorderMetrics.h"

#include <QDebug>

static void qskRegisterBorderMetrics()
{
    qRegisterMetaType< QskBorderMetrics >();
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterBorderMetrics )

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


QskBorderMetrics::QskBorderMetrics( qreal width, qreal radiusX, qreal radiusY ):
    m_widths( ( width >= 0.0 ) ? width : 0.0 ),
    m_widthIsRelative( false ),
    m_radiusIsRelative( false )
{
    setRadius( radiusX, radiusY );
}

QskBorderMetrics::~QskBorderMetrics()
{
}

bool QskBorderMetrics::operator==( const QskBorderMetrics& other ) const
{
    if ( m_widthIsRelative != other.m_widthIsRelative )
        return false;

    if ( m_radiusIsRelative != other.m_radiusIsRelative )
        return false;

    if ( m_widths != other.m_widths )
        return false;

    for ( size_t i = 0; i < 4; i++ )
    {
        if ( m_radii[i] != other.m_radii[i] )
            return false;
    }

    return true;
}

void QskBorderMetrics::setWidthSizeMode( Qt::SizeMode sizeMode )
{
    m_widthIsRelative = ( sizeMode == Qt::RelativeSize );
}

void QskBorderMetrics::setWidths( const QskMargins& widths )
{
    m_widths = widths;
}

void QskBorderMetrics::setWidthAt( Qt::Edges edges, qreal width )
{
    m_widths.setMarginsAt( edges, width );
}

void QskBorderMetrics::setRadiusSizeMode( Qt::SizeMode sizeMode )
{
    m_radiusIsRelative = ( sizeMode == Qt::RelativeSize );
}

void QskBorderMetrics::setRadius(
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

void QskBorderMetrics::setRadius( Qt::Corner corner, qreal radiusX, qreal radiusY )
{
    if ( ( corner >= Qt::TopLeftCorner ) && ( corner <= Qt::BottomRightCorner ) )
    {
        m_radii[ corner ].setWidth( qMax( radiusX, 0.0 ) );
        m_radii[ corner ].setHeight( qMax( radiusY, 0.0 ) );
    }
}

QskBorderMetrics QskBorderMetrics::toAbsolute( const QSizeF& size ) const
{
    if ( !( m_radiusIsRelative || m_widthIsRelative ) )
        return *this;

    QskBorderMetrics absoluted = *this;

    if ( m_radiusIsRelative )
    {
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

                radii.setWidth( qskAbsoluted( size.width(), radii.width() ) );
                radii.setHeight( qskAbsoluted( size.height(), radii.height() ) );
            }
        }
    }

    if ( m_widthIsRelative )
    {
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
    }

    return absoluted;
}

QskBorderMetrics QskBorderMetrics::interpolated(
    const QskBorderMetrics& to, qreal ratio ) const
{
    if ( *this == to )
        return to;

    if ( ( m_widthIsRelative != to.m_widthIsRelative )
        && ( m_radiusIsRelative != to.m_radiusIsRelative ) )
    {
        return to;
    }

    QskMargins widths( to.m_widths );
    if ( m_widthIsRelative == to.m_widthIsRelative )
    {
        widths = m_widths.interpolated( to.m_widths, ratio );
    }

    if ( m_radiusIsRelative == to.m_radiusIsRelative )
    {
        QSizeF radii[4];
        radii[0] = qskInterpolatedSize( m_radii[0], to.m_radii[0], ratio );
        radii[1] = qskInterpolatedSize( m_radii[1], to.m_radii[1], ratio );
        radii[2] = qskInterpolatedSize( m_radii[2], to.m_radii[2], ratio );
        radii[3] = qskInterpolatedSize( m_radii[3], to.m_radii[3], ratio );

        return QskBorderMetrics( to.m_widthIsRelative, widths,
            to.m_radiusIsRelative, radii );
    }
    else
    {
        return QskBorderMetrics( to.m_widthIsRelative, widths,
            to.m_radiusIsRelative, to.m_radii );
    }
}

QVariant QskBorderMetrics::interpolate( const QskBorderMetrics& from,
    const QskBorderMetrics& to, qreal progress )
{
    return QVariant::fromValue( from.interpolated( to, progress ) );
}

#ifndef QT_NO_DEBUG_STREAM

QDebug operator<<( QDebug debug, const QskBorderMetrics& metrics )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "BorderMetrics" << '(';
    debug << metrics.widthSizeMode() << metrics.widths();

    debug << metrics.radiusSizeMode();

    for ( int i = Qt::TopLeftCorner; i <= Qt::BottomRightCorner; i++ )
    {
        const QSizeF r = metrics.radius( static_cast< Qt::Corner >( i ) );
        debug << "(" << r.width() << r.height() << ")";
    }

    debug << ')';

    return debug;
}

#endif

