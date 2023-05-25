/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskStippleMetrics.h"

#include <qhashfunctions.h>
#include <qpen.h>
#include <qvariant.h>

static void qskRegisterStippleMetrics()
{
    qRegisterMetaType< QskStippleMetrics >();

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    QMetaType::registerEqualsComparator< QskStippleMetrics >();
#endif

    QMetaType::registerConverter< QPen, QskStippleMetrics >(
        []( const QPen& pen ) { return QskStippleMetrics( pen ); } );

    QMetaType::registerConverter< Qt::PenStyle, QskStippleMetrics >(
        []( Qt::PenStyle style ) { return QskStippleMetrics( style ); } );
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterStippleMetrics )

QVector< qreal > qskDashPattern( Qt::PenStyle style )
{
    static const QVector< qreal > pattern[] =
    {
        {}, { 1 }, { 4, 2 }, { 1, 2 },
        { 4, 2, 1, 2 }, { 4, 2, 1, 2, 1, 2 }, {}
    };

    return pattern[ style ];
}

static inline qreal qskInterpolated( qreal from, qreal to, qreal ratio )
{
    return from + ( to - from ) * ratio;
}

static inline QVector< qreal > qskInterpolatedSpaces(
    const QVector< qreal >& pattern, qreal progress )
{
    QVector< qreal > interpolated;
    interpolated.reserve( pattern.count() );

    for ( int i = 1; i < pattern.count(); i += 2 )
        interpolated[i] = progress * pattern[i];

    return interpolated;
}

QskStippleMetrics::QskStippleMetrics( Qt::PenStyle penStyle )
    : m_pattern( qskDashPattern( penStyle ) )
{
}

QskStippleMetrics::QskStippleMetrics( const QPen& pen )
    : QskStippleMetrics( pen.style() )
{
    if ( pen.style() == Qt::CustomDashLine )
    {
        m_offset = pen.dashOffset();
        m_pattern = pen.dashPattern();
    }
}

void QskStippleMetrics::setPattern( const QVector< qreal >& pattern )
{
    m_pattern = pattern;
}

void QskStippleMetrics::setOffset( qreal offset ) noexcept
{
    m_offset = offset;
}

QskStippleMetrics QskStippleMetrics::interpolated(
    const QskStippleMetrics& to, qreal progress ) const
{
    if ( *this == to )
        return to;

    const auto offset = qskInterpolated( m_offset, to.m_offset, progress );

    QVector< qreal > pattern;

    if ( isSolid() )
    {
        pattern = qskInterpolatedSpaces( to.m_pattern, progress );
    }
    else if ( to.isSolid() )
    {
        pattern = qskInterpolatedSpaces( m_pattern, 1.0 - progress );
    }
    else
    {
        const auto count = qMax( m_pattern.count(), to.m_pattern.count() );
        pattern.reserve( count );

        for ( int i = 0; i < count; i++ )
        {
            const auto v1 = m_pattern.value( i, 0.0 );
            const auto v2 = to.m_pattern.value( i, 0.0 );

            pattern += qskInterpolated( v1, v2, progress );
        }
    }

    return QskStippleMetrics( pattern, offset );
}

QVariant QskStippleMetrics::interpolate(
    const QskStippleMetrics& from, const QskStippleMetrics& to, qreal progress )
{
    return QVariant::fromValue( from.interpolated( to, progress ) );
}

QskHashValue QskStippleMetrics::hash( QskHashValue seed ) const noexcept
{
    auto hash = qHash( m_offset, seed );
    return qHash( m_pattern, hash );
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskStippleMetrics& metrics )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "QskStippleMetrics" << '(';
    debug << metrics.offset() << ',' << metrics.pattern();
    debug << ')';

    return debug;
}

#endif

#include "moc_QskStippleMetrics.cpp"
