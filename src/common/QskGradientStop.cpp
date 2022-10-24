/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGradientStop.h"
#include "QskRgbValue.h"

#include <qhashfunctions.h>
#include <qvariant.h>
#include <qbrush.h>

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

// some helper functions around QskGradientStops

bool qskIsVisible( const QskGradientStops& stops ) noexcept
{
    for ( const auto& stop : stops )
    {
        const auto& c = stop.color();
        if ( c.isValid() && c.alpha() > 0 )
            return true;
    }

    return false;
}

bool qskIsMonochrome( const QskGradientStops& stops ) noexcept
{
    for ( int i = 1; i < stops.size(); i++ )
    {
        if ( stops[ i ].color() != stops[ 0 ].color() )
            return false;
    }

    return true;
}

QskGradientStops qskTransparentGradientStops( const QskGradientStops& stops, qreal ratio )
{
    auto newStops = stops;

    for ( auto& stop : newStops )
    {
        auto c = stop.color();
        c.setAlpha( c.alpha() * ratio );

        stop.setColor( c );
    }

    return newStops;
}

QVector< QskGradientStop > qskBuildGradientStops( const QGradientStops& qtStops )
{
    QVector< QskGradientStop > stops;
    stops.reserve( qtStops.count() );

    for ( const auto& s : qtStops )
        stops += QskGradientStop( s.first, s.second );

    return stops;
}

template< typename T >
static inline QVector< QskGradientStop > qskCreateStops(
    const QVector< T > colors, bool discrete )
{
    QVector< QskGradientStop > stops;

    const auto count = colors.count();
    if ( count == 0 )
        return stops;

    if ( count == 1 )
    {
        stops.reserve( 2 );

        stops += QskGradientStop( 0.0, colors[0] );
        stops += QskGradientStop( 1.0, colors[0] );

        return stops;
    }

    if ( discrete )
    {
        const auto step = 1.0 / count;
        stops.reserve( 2 * count - 2 );

        stops += QskGradientStop( 0.0, colors[0] );

        for ( int i = 1; i < count; i++ )
        {
            const qreal pos = i * step;
            stops += QskGradientStop( pos, colors[i - 1] );
            stops += QskGradientStop( pos, colors[i] );
        }

        stops += QskGradientStop( 1.0, colors[count - 1] );
    }
    else
    {
        const auto step = 1.0 / ( count - 1 );
        stops.reserve( count );

        stops += QskGradientStop( 0.0, colors[0] );

        for ( int i = 1; i < count - 1; i++ )
            stops += QskGradientStop( i * step, colors[i] );

        stops += QskGradientStop( 1.0, colors[count - 1] );
    }

    return stops;
}

QskGradientStops qskBuildGradientStops( const QVector< QRgb >& colors, bool discrete )
{
    return qskCreateStops( colors, discrete );
}

QskGradientStops qskBuildGradientStops( const QVector< QColor >& colors, bool discrete )
{
    return qskCreateStops( colors, discrete );
}

QGradientStops qskToQGradientStops( const QskGradientStops& stops )
{
    QGradientStops qstops;
    qstops.reserve( stops.count() );

    for ( const auto& stop : stops )
        qstops += { stop.position(), stop.color() };

    return qstops;
}

