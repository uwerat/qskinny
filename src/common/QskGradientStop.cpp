/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGradientStop.h"
#include "QskRgbValue.h"

#include <qhashfunctions.h>
#include <qvariant.h>

#include <algorithm>

static void qskRegisterGradientStop()
{
    qRegisterMetaType< QskGradientStop >();

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    QMetaType::registerEqualsComparator< QskGradientStop >();
#endif
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterGradientStop )

void QskGradientStop::setPosition( qreal position ) noexcept
{
    m_position = position;
}

void QskGradientStop::resetPosition() noexcept
{
    m_position = -1.0;
}

void QskGradientStop::setColor( const QColor& color ) noexcept
{
    m_color = color;
}

void QskGradientStop::resetColor() noexcept
{
    m_color = QColor();
}

void QskGradientStop::setStop( qreal position, const QColor& color ) noexcept
{
    m_position = position;
    m_color = color;
}

QskHashValue QskGradientStop::hash( QskHashValue seed ) const noexcept
{
    auto hash = qHashBits( &m_position, sizeof( m_position ), seed );
    return qHashBits( &m_color, sizeof( m_color ), hash );
}

QColor QskGradientStop::interpolated(
    const QskGradientStop& s1, const QskGradientStop& s2, qreal position ) noexcept
{
    if ( s1.color() == s2.color() )
        return s1.color();

    auto min = &s1;
    auto max = &s2;

    if ( min->position() > max->position() )
        std::swap( min, max );

    if ( position <= min->position() )
        return min->color();

    if ( position >= max->position() )
        return max->color();

    const qreal r = ( position - min->position() ) / ( max->position() - min->position() );
    return QskRgb::interpolated( min->color(), max->color(), r );
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskGradientStop& stop )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << stop.position() << ": ";
    QskRgb::debugColor( debug, stop.color() );

    return debug;
}

#endif

#include "moc_QskGradientStop.cpp"
