/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGradient.h"
#include "QskRgbValue.h"

#include <qhashfunctions.h>
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

static inline QskGradient::Orientation qskOrientation( Qt::Orientation o )
{
    return ( o == Qt::Vertical )
        ? QskGradient::Vertical : QskGradient::Horizontal;
}

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

    return from.orientation() == to.orientation();
}

QskGradient::QskGradient( Orientation orientation ) noexcept
    : m_orientation( orientation )
    , m_isDirty( false )
    , m_isValid( false )
    , m_isMonchrome( true )
    , m_isVisible( false )
{
}

QskGradient::QskGradient( const QColor& color )
    : QskGradient( Vertical )
{
    setStops( color );
}

QskGradient::QskGradient( Qt::Orientation orientation,
        const QColor& startColor, const QColor& stopColor )
    : QskGradient( qskOrientation( orientation ), startColor, stopColor )
{
}

QskGradient::QskGradient( Orientation orientation,
        const QColor& startColor, const QColor& stopColor )
    : QskGradient( orientation )
{
    setStops( startColor, stopColor );
}

QskGradient::QskGradient( Qt::Orientation orientation, const QskGradientStops& stops )
    : QskGradient( qskOrientation( orientation ), stops )
{
}

QskGradient::QskGradient( Orientation orientation, const QskGradientStops& stops )
    : QskGradient( orientation )
{
    setStops( stops );
}

QskGradient::QskGradient( Qt::Orientation orientation, QGradient::Preset preset )
    : QskGradient( qskOrientation( orientation ), preset )
{
}

QskGradient::QskGradient( Orientation orientation, QGradient::Preset preset )
    : QskGradient( orientation )
{
    setStops( qskBuildGradientStops( QGradient( preset ).stops() ) );
}

QskGradient::~QskGradient()
{
}

bool QskGradient::operator==( const QskGradient& other ) const noexcept
{
    return ( m_orientation == other.m_orientation ) && ( m_stops == other.m_stops );
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

void QskGradient::setOrientation( Qt::Orientation orientation ) noexcept
{
    setOrientation( qskOrientation( orientation ) );
}

void QskGradient::setOrientation( Orientation orientation ) noexcept
{
    // does not change m_isDirty
    m_orientation = orientation;
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
        // We simply interpolate stops

        gradient.setOrientation( to.orientation() );

        gradient.setStops( qskInterpolatedGradientStops(
            m_stops, isMonochrome(),
            to.m_stops, to.isMonochrome(), ratio ) );
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

            gradient.setOrientation( orientation() );
            gradient.setStops( qskInterpolatedGradientStops( m_stops, c, r ) );
        }
        else
        {
            const auto r = 2.0 * ( ratio - 0.5 );

            gradient.setOrientation( to.orientation() );
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
    const auto o = orientation();

    auto hash = qHashBits( &o, sizeof( o ), seed );
    for ( const auto& stop : m_stops )
        hash = stop.hash( hash );

    return hash;
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskGradient& gradient )
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

        if ( gradient.isMonochrome() )
        {
            QskRgb::debugColor( debug, gradient.startColor() );
        }
        else
        {
            const char o[] = { 'H', 'V', 'D' };
            debug << o[ gradient.orientation() ] << ", ";

            if ( gradient.stops().count() == 2 )
            {
                QskRgb::debugColor( debug, gradient.startColor() );
                debug << ", ";
                QskRgb::debugColor( debug, gradient.endColor() );
            }
            else
            {
                const auto& stops = gradient.stops();
                for ( int i = 0; i < stops.count(); i++ )
                {
                    if ( i != 0 )
                        debug << ", ";

                    debug << stops[i];
                }
            }
        }
        debug << " )";
    }

    return debug;
}

#endif

#include "moc_QskGradient.cpp"
