/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskCorner.h"
#include <QSizeF>
#include <QDebug>

QskCorner::QskCorner():
    m_radius( 0.0 ),
    m_mode( Qt::AbsoluteSize )
{
}

QskCorner::QskCorner( Qt::SizeMode mode, qreal radius ):
    m_radius( qMax( radius, 0.0 ) ),
    m_mode( mode )
{
}

void QskCorner::setRadius( qreal radius )
{
    m_radius = qMax( radius, 0.0 );
}

qreal QskCorner::radius() const
{
    return m_radius;
}

void QskCorner::setMode( Qt::SizeMode mode )
{
    m_mode = mode;
}

Qt::SizeMode QskCorner::mode() const
{
    return m_mode;
}

bool QskCorner::operator==( const QskCorner& other ) const
{
    return ( m_mode == other.m_mode ) && ( m_radius == other.m_radius );
}

qreal QskCorner::effectiveRadius( const QSizeF& size ) const
{
    return effectiveRadius( size.width(), size.height() );
}

qreal QskCorner::effectiveRadius( qreal width, qreal height ) const
{
    if ( width <= 0.0 || height <= 0.0 )
        return 0.0;

    qreal radius = m_radius;

    if ( m_mode == Qt::RelativeSize )
    {
        // m_radius is a percentage
        radius = qBound( 0.0, radius, 100.0 );
        radius *= 0.5 * qMin( width, height ) / 100.0;
    }

    return radius;
}

#ifndef QT_NO_DEBUG_STREAM

QDebug operator<<( QDebug debug, const QskCorner& corner )
{
    QDebugStateSaver saver( debug );
    debug.nospace();
    debug << "Corner" << '(';
    debug << corner.mode() << "," << corner.radius();
    debug << ')';
    return debug;
}
#endif

#include "moc_QskCorner.cpp"
