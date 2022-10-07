/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Gradient.h"
#include <cassert>

Gradient::Gradient( const Gradient& other ) noexcept
    : m_stops( other.m_stops )
    , m_values{ other.m_values[0], other.m_values[1], other.m_values[2],
        other.m_values[3], other.m_values[4], other.m_values[5] }
    , m_type( other.m_type )
    , m_spread( other.m_spread )
{
}

Gradient& Gradient::operator=( const Gradient& other ) noexcept
{
    m_type = other.m_type;
    m_spread = other.m_spread;
    m_stops = other.m_stops;

    m_values[0] = other.m_values[0];
    m_values[1] = other.m_values[1];
    m_values[2] = other.m_values[2];
    m_values[3] = other.m_values[3];
    m_values[4] = other.m_values[4];
    m_values[5] = other.m_values[5];

    return *this;
}

bool Gradient::operator==( const Gradient& other ) const noexcept
{
    if ( m_type != other.m_type )
        return false;

#if 0
    if ( m_type == QGradient::NoGradient )
        return true;
#endif

    return ( m_spread == other.m_spread )
        && ( m_values[0] == other.m_values[0] )
        && ( m_values[1] == other.m_values[1] )
        && ( m_values[2] == other.m_values[2] )
        && ( m_values[3] == other.m_values[3] )
        && ( m_values[4] == other.m_values[4] )
        && ( m_values[5] == other.m_values[5] )
        && ( m_stops == other.m_stops );
}

void Gradient::setStops( QGradient::Preset preset )
{
    setStops( QGradient( preset ).stops() );
}

void Gradient::setStops( const QGradientStops &stops )
{
    m_stops.resize( stops.count() );

    for ( int i = 0; i < stops.count(); i++ )
        m_stops[i] = { stops[i].first, stops[i].second };
}

void Gradient::setStops( const QVector< QskGradientStop >& stops )
{
    m_stops = stops;
}

void Gradient::setStops( const QColor& c1, const QColor& c2 )
{
    m_stops.resize( 2 );
    m_stops[0] = QskGradientStop( 0.0, c1 );
    m_stops[1] = QskGradientStop( 1.0, c2 );
}

QGradientStops Gradient::qtStops() const
{
    QGradientStops qstops;
    qstops.reserve( m_stops.count() );

    for ( const auto& stop : m_stops )
        qstops += { stop.position(), stop.color() };

    return qstops;
}

void Gradient::setSpread( QGradient::Spread spread )
{
    m_spread = spread;
}

bool Gradient::isMonochrome() const
{
    if ( m_type != QGradient::NoGradient )
    {
        for ( int i = 1; i < m_stops.size(); i++ )
        {
            if ( m_stops[ i ].color() != m_stops[ 0 ].color() )
                return false;
        }
    }

    return true;
}

bool Gradient::isVisible() const
{
    if ( m_type != QGradient::NoGradient )
    {
        for ( const auto& stop : m_stops )
        {
            const auto& c = stop.color();
            if ( c.isValid() && c.alpha() > 0 )
                return true;
        }
    }

    return false;
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const Gradient& gradient )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "Gradient";

    if ( !gradient.isValid() )
    {
        debug << "()";
    }
    else
    {
        debug << "( ";

        debug << gradient.type();
        // ...

        debug << " )";
    }

    return debug;
}

#endif

LinearGradient& Gradient::asLinearGradient()
{
    assert( m_type == QGradient::LinearGradient );
    return *reinterpret_cast< LinearGradient* >( this );
}

const LinearGradient& Gradient::asLinearGradient() const
{
    assert( m_type == QGradient::LinearGradient );
    return *reinterpret_cast< const LinearGradient* >( this );
}

RadialGradient& Gradient::asRadialGradient()
{
    assert( m_type == QGradient::RadialGradient );
    return *reinterpret_cast< RadialGradient* >( this );
}

const RadialGradient& Gradient::asRadialGradient() const
{
    assert( m_type == QGradient::RadialGradient );
    return *reinterpret_cast< const RadialGradient* >( this );
}

ConicGradient& Gradient::asConicGradient()
{
    assert( m_type == QGradient::ConicalGradient );
    return *reinterpret_cast< ConicGradient* >( this );
}

const ConicGradient& Gradient::asConicGradient() const
{
    assert( m_type == QGradient::ConicalGradient );
    return *reinterpret_cast< const ConicGradient* >( this );
}

void LinearGradient::setStart( const QPointF& pos ) noexcept
{
    m_values[0] = pos.x();
    m_values[1] = pos.y();
}

void LinearGradient::setStart( qreal x, qreal y ) noexcept
{
    m_values[0] = x;
    m_values[1] = y;
}

void LinearGradient::setStop( const QPointF& pos ) noexcept
{
    m_values[2] = pos.x();
    m_values[3] = pos.y();
}

void LinearGradient::setStop( qreal x, qreal y ) noexcept
{
    m_values[2] = x;
    m_values[3] = y;
}

void RadialGradient::setCenter( const QPointF& center ) noexcept
{
    m_values[0] = center.x();
    m_values[1] = center.y();
}

void RadialGradient::setCenter( qreal x, qreal y ) noexcept
{
    m_values[0] = x;
    m_values[1] = y;
}

void RadialGradient::setFocalPoint( const QPointF& focalPoint ) noexcept
{
    m_values[3] = focalPoint.x();
    m_values[4] = focalPoint.y();
}

void RadialGradient::setFocalPoint( qreal x, qreal y ) noexcept
{
    m_values[3] = x;
    m_values[4] = y;
}

void RadialGradient::setCenterRadius( qreal radius ) noexcept
{
    m_values[2] = radius;
}

void RadialGradient::setFocalRadius( qreal radius ) noexcept
{
    m_values[5] = radius;
}

void ConicGradient::setCenter( const QPointF& center ) noexcept
{
    m_values[0] = center.x();
    m_values[1] = center.y();
}

void ConicGradient::setCenter( qreal x, qreal y ) noexcept
{
    m_values[0] = x;
    m_values[1] = y;
}

void ConicGradient::setDegrees( qreal degrees ) noexcept
{
    m_values[2] = degrees;
}
