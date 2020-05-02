/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskLayoutHint.h"
#include "QskControl.h"
#include <qnamespace.h>
#include <algorithm>

void QskLayoutHint::setSize( int which, qreal size ) noexcept
{
    switch (which)
    {
        case Qt::MinimumSize:
            m_minimum = size;
            break;

        case Qt::PreferredSize:
            m_preferred = size;
            break;

        case Qt::MaximumSize:
            m_maximum = size;
            break;

        default:
            break;
    }
}

void QskLayoutHint::expandTo( const QskLayoutHint& other ) noexcept
{
    m_minimum = std::max( m_minimum, other.m_minimum );
    m_preferred = std::max( m_preferred, other.m_preferred );
    m_maximum = std::max( m_maximum, other.m_maximum );
}

void QskLayoutHint::normalize() noexcept
{
    m_minimum = std::max( m_minimum, qreal( 0.0 ) );
    m_maximum = std::max( m_minimum, m_maximum );
    m_preferred = qBound( m_minimum, m_preferred, m_maximum );
}

qreal QskLayoutHint::combined( int which, qreal value1, qreal value2 ) noexcept
{
    if ( which == Qt::MaximumSize )
    {
        if ( value1 < 0.0 )
            return value2;

        if ( value2 < 0.0 )
            return value1;

        return std::min( value1, value2 );
    }

    return std::max( value1, value2 );
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

static inline QString qskHintValueString( qreal value )
{
    if ( value >= QskLayoutHint::unlimited )
        return QStringLiteral( "unlimited" );
    else
        return QString::number( value );
}

QDebug operator<<( QDebug debug, const QskLayoutHint& hint )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "LayoutHint" << "( "
        << qskHintValueString( hint.minimum() ) << ", "
        << qskHintValueString( hint.preferred() ) << ", "
        << qskHintValueString( hint.maximum() ) << " )";

    return debug;
}

#endif

#include "moc_QskLayoutHint.cpp"
