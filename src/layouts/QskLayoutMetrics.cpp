/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskLayoutMetrics.h"
#include "QskControl.h"
#include <qnamespace.h>
#include <algorithm>

static void qskRegisterLayoutMetrics()
{
    qRegisterMetaType< QskLayoutMetrics >();

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    QMetaType::registerEqualsComparator< QskLayoutMetrics >();
#endif
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterLayoutMetrics )

void QskLayoutMetrics::setMetric( int which, qreal metric ) noexcept
{
    switch (which)
    {
        case Qt::MinimumSize:
            m_minimum = metric;
            break;

        case Qt::PreferredSize:
            m_preferred = metric;
            break;

        case Qt::MaximumSize:
            m_maximum = metric;
            break;

        default:
            break;
    }
}

void QskLayoutMetrics::expandTo( const QskLayoutMetrics& other ) noexcept
{
    m_minimum = std::max( m_minimum, other.m_minimum );
    m_preferred = std::max( m_preferred, other.m_preferred );
    m_maximum = std::max( m_maximum, other.m_maximum );
}

void QskLayoutMetrics::normalize() noexcept
{
    m_minimum = std::max( m_minimum, qreal( 0.0 ) );
    m_maximum = std::max( m_minimum, m_maximum );
    m_preferred = qBound( m_minimum, m_preferred, m_maximum );
}

qreal QskLayoutMetrics::combined( int which, qreal value1, qreal value2 ) noexcept
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
    if ( value >= QskLayoutMetrics::unlimited )
        return QStringLiteral( "unlimited" );
    else
        return QString::number( value );
}

QDebug operator<<( QDebug debug, const QskLayoutMetrics& metrics )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "LayoutMetrics" << "( "
        << qskHintValueString( metrics.minimum() ) << ", "
        << qskHintValueString( metrics.preferred() ) << ", "
        << qskHintValueString( metrics.maximum() ) << " )";

    return debug;
}

#endif

#include "moc_QskLayoutMetrics.cpp"
