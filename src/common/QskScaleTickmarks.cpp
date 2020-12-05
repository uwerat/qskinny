/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskScaleTickmarks.h"
#include <algorithm>

int QskScaleTickmarks::tickCount() const noexcept
{
    return m_ticks[ MajorTick ].count()
        + m_ticks[ MediumTick ].count()
        + m_ticks[ MinorTick ].count();
}

QskScaleTickmarks::QskScaleTickmarks()
{
}

QskScaleTickmarks::~QskScaleTickmarks()
{
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

#include "moc_QskScaleTickmarks.cpp"
