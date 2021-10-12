/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskArcMetrics.h"

#include <qhashfunctions.h>
#include <qvariant.h>

static void qskRegisterArcMetrics()
{
    qRegisterMetaType< QskArcMetrics >();
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterArcMetrics )

void QskArcMetrics::setSizeMode( Qt::SizeMode sizeMode ) noexcept
{
    m_sizeMode = sizeMode;
}

void QskArcMetrics::setStartAngle( qreal angle ) noexcept
{
    m_startAngle = angle;
}

void QskArcMetrics::setEndAngle( qreal angle ) noexcept
{
    m_endAngle = angle;
}

void QskArcMetrics::setWidth( qreal width ) noexcept
{
    m_width = width;
}

uint QskArcMetrics::hash( uint seed ) const noexcept
{
    uint hash = qHash( m_startAngle, seed );
    hash = qHash( m_endAngle, hash );
    hash = qHash( m_width, hash );
    const int mode = m_sizeMode;
    return qHashBits( &mode, sizeof( mode ), hash );
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskArcMetrics& metrics )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "Arc" << '(';
    debug << "start angle:" << metrics.startAngle();
    debug << ", end angle:" << metrics.endAngle();
    debug << ", size mode:" << metrics.sizeMode();
    debug << ", width:" << metrics.width();
    debug << ')';

    return debug;
}

#endif

#include "moc_QskArcMetrics.cpp"
