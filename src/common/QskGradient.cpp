/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGradient.h"
#include "QskRgbValue.h"

#include <qvariant.h>

static void qskRegisterGradient()
{
    qRegisterMetaType< QskGradient >();

#if QT_VERSION < QT_VERSION_CHECK( 6, 0, 0 )
    QMetaType::registerEqualsComparator< QskGradient >();
#endif

    QMetaType::registerConverter< QColor, QskGradient >(
        []( const QColor& color ) { return QskGradient( color ); } );
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterGradient )

static inline bool qskIsGradientValid( const QskGradientStops& stops )
{
    if ( stops.isEmpty() )
        return false;

    if ( stops.first().position() < 0.0 || stops.last().position() > 1.0 )
        return false;

    if ( !stops.first().color().isValid() )
        return false;

    for ( int i = 1; i < stops.size(); i++ )
    {
        if ( stops[ i ].position() < stops[ i - 1 ].position() )
            return false;

        if ( !stops[ i ].color().isValid() )
            return false;
    }

    return true;
}

static inline bool qskCanBeInterpolated( const QskGradient& from, const QskGradient& to )
{
    if ( from.isMonochrome() || to.isMonochrome() )
        return true;

    return from.type() == to.type();
}

QskGradient::QskGradient( const QColor& color )
{
    setStops( color );
}

QskGradient::QskGradient( const QColor& color1, const QColor& color2 )
{
    setStops( color1, color2 );
}

QskGradient::QskGradient( QGradient::Preset preset )
{
    setStops( qskBuildGradientStops( QGradient( preset ).stops() ) );
}

QskGradient::QskGradient( const QVector< QskGradientStop >& stops )
{
    setStops( stops );
}

QskGradient::QskGradient( const QskGradient& other ) noexcept
    : m_stops( other.m_stops )
    , m_values{ other.m_values[0], other.m_values[1],
        other.m_values[2], other.m_values[3], }
    , m_type( other.m_type )
    , m_spread( other.m_spread )
    , m_isDirty( other.m_isDirty )
    , m_isValid( other.m_isValid )
    , m_isMonchrome( other.m_isMonchrome )
    , m_isVisible( other.m_isVisible )
{
}

QskGradient::~QskGradient()
{
}

QskGradient& QskGradient::operator=( const QskGradient& other ) noexcept
{
    m_type = other.m_type;
    m_spread = other.m_spread;
    m_stops = other.m_stops;

    m_values[0] = other.m_values[0];
    m_values[1] = other.m_values[1];
    m_values[2] = other.m_values[2];
    m_values[3] = other.m_values[3];

    m_isDirty = other.m_isDirty;
    m_isValid = other.m_isValid;
    m_isMonchrome = other.m_isMonchrome;
    m_isVisible = other.m_isVisible;

    return *this;
}

bool QskGradient::operator==( const QskGradient& other ) const noexcept
{
    return ( m_type == other.m_type )
           && ( m_spread == other.m_spread )
           && ( m_values[0] == other.m_values[0] )
           && ( m_values[1] == other.m_values[1] )
           && ( m_values[2] == other.m_values[2] )
           && ( m_values[3] == other.m_values[3] )
           && ( m_stops == other.m_stops );
}

void QskGradient::updateStatusBits() const
{
    // doing all bits in one loop ?
    m_isValid = qskIsGradientValid( m_stops );

    if ( m_isValid )
    {
        m_isMonchrome = qskIsMonochrome( m_stops );
        m_isVisible = qskIsVisible( m_stops );
    }
    else
    {
        m_isMonchrome = true;
        m_isVisible = false;
    }

    m_isDirty = false;
}

bool QskGradient::isValid() const noexcept
{
    if ( m_isDirty )
        updateStatusBits();

    return m_isValid;
}

bool QskGradient::isMonochrome() const noexcept
{
    if ( m_isDirty )
        updateStatusBits();

    return m_isMonchrome;
}

bool QskGradient::isVisible() const noexcept
{
    if ( m_isDirty )
        updateStatusBits();

    return m_isVisible;
}

void QskGradient::setStops( const QColor& color )
{
    m_stops = { { 0.0, color }, { 1.0, color } };
    m_isDirty = true;
}

void QskGradient::setStops( const QColor& color1, const QColor& color2 )
{
    m_stops = { { 0.0, color1 }, { 1.0, color2 } };
    m_isDirty = true;
}

void QskGradient::setStops( QGradient::Preset preset )
{
    const auto stops = qskBuildGradientStops( QGradient( preset ).stops() );
    setStops( stops );
}

void QskGradient::setStops( const QskGradientStops& stops )
{
    if ( !stops.isEmpty() && !qskIsGradientValid( stops ) )
    {
        qWarning( "Invalid gradient stops" );
        m_stops.clear();
    }
    else
    {
        m_stops = stops;
    }

    m_isDirty = true;
}

int QskGradient::stepCount() const noexcept
{
    if ( !isValid() )
        return 0;

    int steps = m_stops.count() - 1;

    if ( m_stops.first().position() > 0.0 )
        steps++;

    if ( m_stops.last().position() < 1.0 )
        steps++;

    return steps;
}

qreal QskGradient::stopAt( int index ) const noexcept
{
    if ( index < 0 || index >= m_stops.size() )
        return -1.0;

    return m_stops[ index ].position();
}

bool QskGradient::hasStopAt( qreal value ) const noexcept
{
    // better use binary search TODO ...
    for ( auto& stop : m_stops )
    {
        if ( stop.position() == value )
            return true;

        if ( stop.position() > value )
            break;
    }

    return false;
}

QColor QskGradient::colorAt( int index ) const noexcept
{
    if ( index >= m_stops.size() )
        return QColor();

    return m_stops[ index ].color();
}

void QskGradient::setAlpha( int alpha )
{
    for ( auto& stop : m_stops )
    {
        auto c = stop.color();
        if ( c.isValid() && c.alpha() )
        {
            c.setAlpha( alpha );
            stop.setColor( c );
        }
    }

    m_isDirty = true;
}

void QskGradient::setSpread( QGradient::Spread spread )
{
    m_spread = spread;
}

void QskGradient::reverse()
{
    if ( isMonochrome() )
        return;

    std::reverse( m_stops.begin(), m_stops.end() );
    for( auto& stop : m_stops )
        stop.setPosition( 1.0 - stop.position() );
}

QskGradient QskGradient::reversed() const
{
    auto gradient = *this;
    gradient.reverse();

    return gradient;
}

QskGradient QskGradient::extracted( qreal from, qreal to ) const
{
    auto gradient = *this;

    if ( !isValid() || ( from > to ) || ( from > 1.0 ) )
    {
        gradient.clearStops();
    }
    else if ( isMonochrome() )
    {
        from = qMax( from, 0.0 );
        to = qMin( to, 1.0 );

        const auto color = m_stops.first().color();

        gradient.setStops( { { from, color }, { to, color } } );
    }
    else
    {
        gradient.setStops( qskExtractedGradientStops( m_stops, from, to ) );
    }

    return gradient;
}

QskGradient QskGradient::interpolated( const QskGradient& to, qreal ratio ) const
{
    if ( !isValid() && !to.isValid() )
        return to;

    QskGradient gradient;

    if ( qskCanBeInterpolated( *this, to ) )
    {
        // We simply interpolate stops and values

        gradient = to;

        gradient.setStops( qskInterpolatedGradientStops(
            m_stops, isMonochrome(),
            to.m_stops, to.isMonochrome(), ratio ) );

        for ( uint i = 0; i < sizeof( m_values ) / sizeof( m_values[0] ); i++ )
            gradient.m_values[i] = m_values[i] + ratio * ( to.m_values[i] - m_values[i] );
    }
    else
    {
        /*
            The interpolation is devided into 2 steps. First we
            interpolate into a monochrome gradient and then
            recolor the gradient towards the target gradient
            This will always result in a smooth transition - even, when
            interpolating between different gradient types
         */

        const auto c = QskRgb::interpolated( startColor(), to.startColor(), 0.5 );

        if ( ratio < 0.5 )
        {
            const auto r = 2.0 * ratio;

            gradient = *this;
            gradient.setStops( qskInterpolatedGradientStops( m_stops, c, r ) );
        }
        else
        {
            const auto r = 2.0 * ( ratio - 0.5 );

            gradient = to;
            gradient.setStops( qskInterpolatedGradientStops( c, to.m_stops, r ) );
        }
    }

    return gradient;
}

QVariant QskGradient::interpolate(
    const QskGradient& from, const QskGradient& to, qreal progress )
{
    return QVariant::fromValue( from.interpolated( to, progress ) );
}

void QskGradient::clearStops()
{
    if ( !m_stops.isEmpty() )
    {
        m_stops.clear();
        m_isDirty = true;
    }
}

QskHashValue QskGradient::hash( QskHashValue seed ) const
{
    auto hash = qHashBits( &m_type, sizeof( m_type ), seed );

    if ( m_type != Stops )
        hash = qHashBits( m_values, sizeof( m_values ), seed );

    for ( const auto& stop : m_stops )
        hash = stop.hash( hash );

    return hash;
}

#include "QskLinearGradient.h"
#include "QskRadialGradient.h"
#include "QskConicGradient.h"

QskLinearGradient& QskGradient::asLinearGradient()
{
    assert( m_type == QskGradient::Linear );
    return *reinterpret_cast< QskLinearGradient* >( this );
}

const QskLinearGradient& QskGradient::asLinearGradient() const
{
    assert( m_type == QskGradient::Linear );
    return *reinterpret_cast< const QskLinearGradient* >( this );
}

QskRadialGradient& QskGradient::asRadialGradient()
{
    assert( m_type == QskGradient::Radial );
    return *reinterpret_cast< QskRadialGradient* >( this );
}

const QskRadialGradient& QskGradient::asRadialGradient() const
{
    assert( m_type == QskGradient::Radial );
    return *reinterpret_cast< const QskRadialGradient* >( this );
}

QskConicGradient& QskGradient::asConicGradient()
{
    assert( m_type == QskGradient::Conic );
    return *reinterpret_cast< QskConicGradient* >( this );
}

const QskConicGradient& QskGradient::asConicGradient() const
{
    assert( m_type == QskGradient::Conic );
    return *reinterpret_cast< const QskConicGradient* >( this );
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskGradient& gradient )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "QskGradient( ";

    switch ( gradient.type() )
    {
        case QskGradient::Linear:
        {
            debug << "L(";

            const auto& g = gradient.asLinearGradient();
            debug << g.start().x() << "," << g.start().y()
                << "," << g.stop().x() << "," << g.stop().y() << ")";

            break;
        }

        case QskGradient::Radial:
        {
            debug << "R(";

            const auto& g = gradient.asRadialGradient();

            debug << g.center().x() << "," << g.center().y()
                << "," << g.radius() << ")";

            break;
        }

        case QskGradient::Conic:
        {
            debug << "C(";

            const auto& g = gradient.asConicGradient();

            debug << g.center().x() << "," << g.center().y()
                << ",[" << g.startAngle() << "," << g.spanAngle() << "])";
            break;
        }

        case QskGradient::Stops:
        {
            debug << "S()";
            break;
        }
    }

    if ( gradient.stops().isEmpty() )
    {
        debug << " ()";
    }
    else
    {
        if ( gradient.isMonochrome() )
        {
            debug << ' ';
            const auto color = gradient.stops().first().color();
            QskRgb::debugColor( debug, color );
        }
        else
        {
            debug << " ( ";

            const auto& stops = gradient.stops();
            for ( int i = 0; i < stops.count(); i++ )
            {
                if ( i != 0 )
                    debug << ", ";

                debug << stops[i];
            }

            debug << " )";
        }
    }

    switch( gradient.spread() )
    {
        case QGradient::RepeatSpread:
            debug << " RP";
            break;

        case QGradient::ReflectSpread:
            debug << " RF";
            break;

        case QGradient::PadSpread:
            break;
    }

    debug << " )";

    return debug;
}

#endif

#include "moc_QskGradient.cpp"
