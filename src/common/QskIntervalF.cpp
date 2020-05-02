/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskIntervalF.h"
#include <algorithm>

void QskIntervalF::unite( const QskIntervalF& other ) noexcept
{
    if ( isValid() )
    {
        if ( other.isValid() )
        {
            if ( other.m_lowerBound < m_lowerBound )
            {
                m_lowerBound = other.m_lowerBound;
            }
            else if ( other.m_upperBound > m_upperBound )
            {
                m_upperBound = other.m_upperBound;
            }
        }
    }
    else
    {
        if ( other.isValid() )
        {
            m_lowerBound = other.m_lowerBound;
            m_upperBound = other.m_upperBound;
        }
    }
}

QskIntervalF QskIntervalF::united( const QskIntervalF& other ) const noexcept
{
    if ( isValid() )
    {
        if ( other.isValid() )
        {
            const auto min = std::min( m_lowerBound, other.m_lowerBound );
            const auto max = std::max( m_upperBound, other.m_upperBound );

            return QskIntervalF( min, max );
        }
    }
    else
    {
        if ( other.isValid() )
            return other;
    }

    return *this;
}

QskIntervalF QskIntervalF::intersected( const QskIntervalF& other ) const noexcept
{
    if ( !isValid() )
        return *this;

    if ( !other.isValid() )
        return other;

    if ( m_lowerBound <= other.m_lowerBound )
    {
        if ( m_upperBound < other.m_lowerBound )
            return QskIntervalF();

        const qreal max = std::min( m_upperBound, other.m_upperBound );
        return QskIntervalF( other.m_lowerBound, max );
    }
    else
    {
        if ( other.m_upperBound < m_lowerBound )
            return QskIntervalF();

        const qreal max = std::min( m_upperBound, other.m_upperBound );
        return QskIntervalF( m_lowerBound, max );
    }
}

bool QskIntervalF::intersects( const QskIntervalF& other ) const noexcept
{
    if ( !isValid() || !other.isValid() )
        return false;

    if ( m_lowerBound <= other.m_lowerBound )
        return m_upperBound >= other.m_lowerBound;
    else
        return m_lowerBound <= other.m_upperBound;
}

void QskIntervalF::extend( qreal value ) noexcept
{
    if ( !isValid() )
    {
        m_lowerBound = value;
        m_upperBound = value;
    }
    else if ( value < m_lowerBound )
    {
        m_lowerBound = value;
    }
    else if ( value >= m_upperBound )
    {
        m_upperBound = value;
    }
}

QskIntervalF QskIntervalF::extended( qreal value ) const noexcept
{
    if ( !isValid() )
        return QskIntervalF( value, value );

    const qreal lower = std::min( value, m_lowerBound );
    const qreal upper = std::max( value, m_upperBound );

    return QskIntervalF( lower, upper );
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskIntervalF& interval )
{
    debug.nospace() << "QskIntervalF("
        << interval.lowerBound() << "," << interval.upperBound() << ")";

    return debug.space();
}

#endif

#include "moc_QskIntervalF.cpp"
