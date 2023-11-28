/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskTickmarks.h"
#include <algorithm>

static void qskRegisterTickmarks()
{
    qRegisterMetaType< QskTickmarks >();

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    QMetaType::registerEqualsComparator< QskTickmarks >();
#endif
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterTickmarks )

QskTickmarks::QskTickmarks()
{
}

QskTickmarks::QskTickmarks( const QVector< qreal >& minorTicks,
        const QVector< qreal >& mediumTicks, const QVector< qreal >& majorTicks )
    : m_ticks{ minorTicks, mediumTicks, majorTicks }
{
}

QskTickmarks::~QskTickmarks()
{
}

int QskTickmarks::tickCount() const noexcept
{
    const auto count = m_ticks[ MajorTick ].count()
        + m_ticks[ MediumTick ].count()
        + m_ticks[ MinorTick ].count();

    return static_cast< int >( count );
}


int QskTickmarks::tickCount( TickType type ) const noexcept
{
    return static_cast< int >( m_ticks[ type ].count() );
}

QVector< qreal > QskTickmarks::ticks( TickType type ) const noexcept
{
    return m_ticks[ type ];
}

void QskTickmarks::setTicks(TickType type, const QVector< qreal >& ticks )
{
    m_ticks[ type ] = ticks;
}

qreal QskTickmarks::tickAt( TickType type, int index ) const
{
    return m_ticks[ type ].at( index );
}

void QskTickmarks::reset()
{
    m_ticks[ 0 ].clear();
    m_ticks[ 1 ].clear();
    m_ticks[ 2 ].clear();
}

void QskTickmarks::invert()
{
    std::reverse( m_ticks[ 0 ].begin(), m_ticks[ 0 ].end() );
    std::reverse( m_ticks[ 1 ].begin(), m_ticks[ 1 ].end() );
    std::reverse( m_ticks[ 2 ].begin(), m_ticks[ 2 ].end() );
}

QskHashValue QskTickmarks::hash( QskHashValue seed ) const noexcept
{
    seed = qHash( m_ticks[0], seed );
    seed = qHash( m_ticks[1], seed );
    seed = qHash( m_ticks[2], seed );

    return seed;
}

bool QskTickmarks::operator==( const QskTickmarks& other ) const noexcept
{
    return ( m_ticks[ 0 ] == other.m_ticks[ 0 ] )
        && ( m_ticks[ 1 ] == other.m_ticks[ 1 ] )
        && ( m_ticks[ 2 ] == other.m_ticks[ 2 ] );
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskTickmarks& tickmarks )
{
    debug << tickmarks.majorTicks()
        << tickmarks.mediumTicks() << tickmarks.minorTicks();

    return debug;
}

#endif

#include "moc_QskTickmarks.cpp"
