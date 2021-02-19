/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskScaleTickmarks.h"
#include <algorithm>

static void qskRegisterTickmarks()
{
    qRegisterMetaType< QskScaleTickmarks >();
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterTickmarks )

QskScaleTickmarks::QskScaleTickmarks()
{
}

QskScaleTickmarks::~QskScaleTickmarks()
{
}

int QskScaleTickmarks::tickCount() const noexcept
{
    return m_ticks[ MajorTick ].count()
        + m_ticks[ MediumTick ].count()
        + m_ticks[ MinorTick ].count();
}


int QskScaleTickmarks::tickCount( TickType type ) const noexcept
{
    return m_ticks[ type ].count();
}

QVector< qreal > QskScaleTickmarks::ticks( TickType type ) const noexcept
{
    return m_ticks[ type ];
}

void QskScaleTickmarks::setTicks(TickType type, const QVector< qreal >& ticks )
{
    m_ticks[ type ] = ticks;
}

qreal QskScaleTickmarks::tickAt( TickType type, int index ) const
{
    return m_ticks[ type ].at( index );
}

void QskScaleTickmarks::reset()
{
    m_ticks[ 0 ].clear();
    m_ticks[ 1 ].clear();
    m_ticks[ 2 ].clear();
}

void QskScaleTickmarks::invert()
{
    std::reverse( m_ticks[ 0 ].begin(), m_ticks[ 0 ].end() );
    std::reverse( m_ticks[ 1 ].begin(), m_ticks[ 1 ].end() );
    std::reverse( m_ticks[ 2 ].begin(), m_ticks[ 2 ].end() );
}

uint QskScaleTickmarks::hash( uint seed ) const
{
    seed = qHash( m_ticks[0], seed );
    seed = qHash( m_ticks[1], seed );
    seed = qHash( m_ticks[2], seed );

    return seed;
}

bool QskScaleTickmarks::operator==( const QskScaleTickmarks& other ) const noexcept
{
    return ( m_ticks[ 0 ] == other.m_ticks[ 0 ] )
        && ( m_ticks[ 1 ] == other.m_ticks[ 1 ] )
        && ( m_ticks[ 2 ] == other.m_ticks[ 2 ] );
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskScaleTickmarks& tickmarks )
{
    debug << tickmarks.majorTicks()
        << tickmarks.mediumTicks() << tickmarks.minorTicks();

    return debug;
}

#endif

#include "moc_QskScaleTickmarks.cpp"
