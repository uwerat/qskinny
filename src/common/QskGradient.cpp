/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGradient.h"
#include "QskRgbValue.h"
#include "QskGradientDirection.h"
#include "QskFunctions.h"

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

    if ( m_isVisible )
    {
        switch( m_type )
        {
            case Linear:
            {
                m_isVisible = !( qskFuzzyCompare( m_values[0], m_values[2] )
                    && qskFuzzyCompare( m_values[1], m_values[3] ) );
                break;
            }

            case Radial:
            {
                m_isVisible = m_values[2] > 0.0; // radius
                break;
            }

            case Conic:
            {
                m_isVisible = !qFuzzyIsNull( m_values[3] ); // spanAngle
                break;
            }

            default:
                break;
        }
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

void QskGradient::setSpread( Spread spread )
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

        const auto stops = qskInterpolatedGradientStops(
            m_stops, isMonochrome(), to.m_stops, to.isMonochrome(), ratio );

        gradient.setStops( stops );

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

void QskGradient::setLinearDirection( Qt::Orientation orientation )
{
    setLinearDirection( QskLinearDirection( orientation ) );
}

void QskGradient::setLinearDirection( qreal x1, qreal y1, qreal x2, qreal y2 )
{
    setLinearDirection( QskLinearDirection( x1, y1, x2, y2 ) );
}

void QskGradient::setLinearDirection( const QskLinearDirection& direction )
{
    m_type = Linear;

    m_values[0] = direction.x1();
    m_values[1] = direction.y1();
    m_values[2] = direction.x2();
    m_values[3] = direction.y2();
}

QskLinearDirection QskGradient::linearDirection() const
{
    Q_ASSERT( m_type == Linear );

    if ( m_type != Linear )
        return QskLinearDirection( 0.0, 0.0, 0.0, 0.0 );

    return QskLinearDirection( m_values[0], m_values[1], m_values[2], m_values[3] );
}

void QskGradient::setRadialDirection( const qreal x, qreal y, qreal radius )
{
    setRadialDirection( QskRadialDirection( x, y, radius ) );
}

void QskGradient::setRadialDirection( const QskRadialDirection& direction )
{
    m_type = Radial;

    m_values[0] = direction.center().x();
    m_values[1] = direction.center().y();
    m_values[2] = direction.radius();
    m_values[3] = 0.0;
}

QskRadialDirection QskGradient::radialDirection() const
{
    Q_ASSERT( m_type == Radial );

    if ( m_type != Radial )
        return QskRadialDirection( 0.5, 0.5, 0.0 );

    return QskRadialDirection( m_values[0], m_values[1], m_values[2] );
}

void QskGradient::setConicDirection( qreal x, qreal y )
{
    setConicDirection( QskConicDirection( x, y ) );
}

void QskGradient::setConicDirection( qreal x, qreal y,
    qreal startAngle, qreal spanAngle )
{
    setConicDirection( QskConicDirection( x, y, startAngle, spanAngle ) );
}

void QskGradient::setConicDirection( const QskConicDirection& direction )
{
    m_type = Conic;

    m_values[0] = direction.center().x();
    m_values[1] = direction.center().y();
    m_values[2] = direction.startAngle();
    m_values[3] = direction.spanAngle();
}

QskConicDirection QskGradient::conicDirection() const
{
    Q_ASSERT( m_type == Conic );

    if ( m_type != Conic )
        return QskConicDirection( 0.5, 0.5, 0.0, 0.0 );

    return QskConicDirection( m_values[0], m_values[1], m_values[2], m_values[3] );
}

void QskGradient::setDirection( Type type )
{
    if ( type == m_type )
        return;

    switch( type )
    {
        case Linear:
            setLinearDirection( QskLinearDirection() );
            break;

        case Radial:
            setRadialDirection( QskRadialDirection() );
            break;

        case Conic:
            setConicDirection( QskConicDirection() );
            break;

        case Stops:
            resetDirection();
            break;
    }
}

void QskGradient::resetDirection()
{
    m_type = Stops;
    m_values[0] = m_values[1] = m_values[2] = m_values[3] = 0.0;
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskGradient& gradient )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "QskGradient(";

    switch ( gradient.type() )
    {
        case QskGradient::Linear:
        {
            debug << " L(";

            const auto dir = gradient.linearDirection();
            debug << dir.start().x() << "," << dir.start().y()
                << "," << dir.stop().x() << "," << dir.stop().y() << ")";

            break;
        }

        case QskGradient::Radial:
        {
            debug << " R(";

            const auto dir = gradient.radialDirection();

            debug << dir.center().x() << "," << dir.center().y()
                << "," << dir.radius() << ")";

            break;
        }

        case QskGradient::Conic:
        {
            debug << " C(";

            const auto dir = gradient.conicDirection();

            debug << dir.center().x() << "," << dir.center().y()
                << ",[" << dir.startAngle() << "," << dir.spanAngle() << "])";
            break;
        }

        case QskGradient::Stops:
        {
            break;
        }
    }

    if ( !gradient.stops().isEmpty() )
    {
        if ( gradient.isMonochrome() )
        {
            debug << ' ';
            QskRgb::debugColor( debug, gradient.rgbStart() );
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
        case QskGradient::RepeatSpread:
            debug << " RP";
            break;

        case QskGradient::ReflectSpread:
            debug << " RF";
            break;

        case QskGradient::PadSpread:
            break;
    }

    debug << " )";

    return debug;
}

#endif

#include "moc_QskGradient.cpp"
