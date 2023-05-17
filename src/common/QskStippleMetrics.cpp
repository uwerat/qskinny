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
}

static inline QVector< qreal > qskDashPattern( const Qt::PenStyle& style )
{
    static QVector< qreal > pattern[] =
    {
        {}, { 1 }, { 4, 2 }, { 1, 2 },
        { 4, 2, 1, 2 }, { 4, 2, 1, 2, 1, 2 }, {}
    };

    return pattern[ style ];
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterStippleMetrics )

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
