/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGradient.h"
#include "QskRgbValue.h"

#include <qhashfunctions.h>
#include <qvariant.h>

#include <algorithm>

static void qskRegisterGradient()
{
    qRegisterMetaType< QskGradient >();
    qRegisterMetaType< QskGradientStop >();

    QMetaType::registerConverter< QColor, QskGradient >(
        []( const QColor& color ) { return QskGradient( color ); } );
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterGradient )

static inline QskGradient::Orientation qskOrientation( Qt::Orientation o )
{
    return ( o == Qt::Vertical )
        ? QskGradient::Vertical : QskGradient::Horizontal;
}

static inline bool qskIsGradientValid( const QVector< QskGradientStop >& stops )
{
    if ( stops.size() < 2 )
        return false;

    if ( stops.first().position() != 0.0 || stops.last().position() != 1.0 )
    {
        return false;
    }

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

static inline bool qskIsMonochrome( const QVector< QskGradientStop >& stops )
{
    for ( int i = 1; i < stops.size(); i++ )
    {
        if ( stops[ i ].color() != stops[ 0 ].color() )
            return false;
    }

    return true;
}

static inline QColor qskInterpolated(
    const QskGradientStop& s1, const QskGradientStop& s2, qreal pos )
{
    if ( s1.color() == s2.color() )
        return s1.color();

    const qreal ratio = ( pos - s1.position() ) / ( s2.position() - s1.position() );
    return QskRgb::interpolated( s1.color(), s2.color(), ratio );
}

static inline bool qskComparePositions(
    const QVector< QskGradientStop >& s1, const QVector< QskGradientStop >& s2 )
{
    if ( s1.count() != s2.count() )
        return false;

    // the first is always at 0.0, the last at 1.0
    for ( int i = 1; i < s1.count() - 1; i++ )
    {
        if ( s1[ i ].position() != s2[ i ].position() )
            return false;
    }

    return true;
}

static inline QVector< QskGradientStop > qskExpandedStops(
    const QVector< QskGradientStop >& s1, const QVector< QskGradientStop >& s2 )
{
    // expand s1 by stops matching to the positions from s2

    if ( qskComparePositions( s1, s2 ) )
        return s1;

    QVector< QskGradientStop > stops;

    stops += s1.first();

    int i = 1, j = 1;
    while ( ( i < s1.count() - 1 ) || ( j < s2.count() - 1 ) )
    {
        if ( s1[ i ].position() < s2[ j ].position() )
        {
            stops += s1[ i++ ];
        }
        else
        {
            const qreal pos = s2[ j++ ].position();
            stops += QskGradientStop( pos, qskInterpolated( s1[ i - 1 ], s1[ i ], pos ) );
        }
    }

    stops += s1.last();

    return stops;
}

static inline QVector< QskGradientStop > qskExtractedStops(
    const QVector< QskGradientStop >& stops, qreal from, qreal to )
{
    QVector< QskGradientStop > extracted;

    if ( from == to )
        extracted.reserve( 2 );
    else
        extracted.reserve( stops.size() );

    int i = 0;

    if ( from == 0.0 )
    {
        extracted += QskGradientStop( 0.0, stops[i++].color() );
    }
    else
    {
        for ( i = 1; i < stops.count(); i++ )
        {
            if ( stops[i].position() > from )
                break;
        }

        const auto color =
            QskGradientStop::interpolated( stops[i-1], stops[i], from );

        extracted += QskGradientStop( 0.0, color );
    }

    for ( ; i < stops.count(); i++ )
    {
        const auto& s = stops[i];

        if ( s.position() >= to )
            break;

        const auto pos = ( s.position() - from ) / ( to - from );
        extracted += QskGradientStop( pos, s.color() );
    }

    const auto color = QskGradientStop::interpolated( stops[i-1], stops[i], to );
    extracted += QskGradientStop( 1.0, color );

    return extracted;
}

void QskGradientStop::setPosition( qreal position )
{
    m_position = position;
}

void QskGradientStop::resetPosition()
{
    m_position = -1.0;
}

void QskGradientStop::setColor( const QColor& color )
{
    m_color = color;
}

void QskGradientStop::resetColor()
{
    m_color = QColor();
}

void QskGradientStop::setStop( qreal position, const QColor& color )
{
    m_position = position;
    m_color = color;
}

uint QskGradientStop::hash( uint seed ) const
{
    uint hash = qHashBits( &m_position, sizeof( m_position ), seed );
    return qHashBits( &m_color, sizeof( m_color ), hash );
}

QColor QskGradientStop::interpolated(
    const QskGradientStop& s1, const QskGradientStop& s2, qreal position )
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

QskGradient::QskGradient()
    : m_orientation( Vertical )
{
}

QskGradient::QskGradient( const QColor& color )
    : m_orientation( Vertical )
{
    setColor( color );
}

QskGradient::QskGradient( Qt::Orientation orientation,
        const QColor& startColor, const QColor& stopColor )
    : QskGradient( qskOrientation( orientation ), startColor, stopColor )
{
}

QskGradient::QskGradient( Orientation orientation,
        const QColor& startColor, const QColor& stopColor )
    : m_orientation( orientation )
{
    setColors( startColor, stopColor );
}

QskGradient::QskGradient( Qt::Orientation orientation,
        const QVector< QskGradientStop >& stops )
    : QskGradient( qskOrientation( orientation ), stops )
{
}

QskGradient::QskGradient( Orientation orientation,
        const QVector< QskGradientStop >& stops )
    : m_orientation( orientation )
    , m_stops( stops )
{
}

QskGradient::~QskGradient()
{
}

bool QskGradient::isValid() const
{
    return qskIsGradientValid( m_stops );
}

void QskGradient::invalidate()
{
    m_stops.clear();
}

bool QskGradient::isMonochrome() const
{
    if ( !qskIsGradientValid( m_stops ) )
        return true;

    return qskIsMonochrome( m_stops );
}

bool QskGradient::isVisible() const
{
    if ( isValid() )
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

void QskGradient::setOrientation( Qt::Orientation orientation )
{
    setOrientation( qskOrientation( orientation ) );
}

void QskGradient::setOrientation( Orientation orientation )
{
    m_orientation = orientation;
}

QskGradient::Orientation QskGradient::orientation() const
{
    return m_orientation;
}

void QskGradient::setColor( const QColor& color )
{
    m_stops.reserve( 2 );

    m_stops.append( QskGradientStop( 0.0, color ) );
    m_stops.append( QskGradientStop( 1.0, color ) );
}

void QskGradient::setColors( const QColor& startColor, const QColor& stopColor )
{
    m_stops.reserve( 2 );

    m_stops.append( QskGradientStop( 0.0, startColor ) );
    m_stops.append( QskGradientStop( 1.0, stopColor ) );
}

void QskGradient::setStops( const QVector< QskGradientStop >& stops )
{
    if ( !qskIsGradientValid( stops ) )
    {
        qWarning( "Invalid gradient stops" );
        invalidate();
        return;
    }

    m_stops = stops;
}

QVector< QskGradientStop > QskGradient::stops() const
{
    return m_stops;
}

int QskGradient::stopCount() const
{
    return m_stops.count();
}

void QskGradient::setStopAt( int index, qreal stop )
{
    if ( stop < 0.0 || stop > 1.0 )
    {
        qWarning( "Invalid gradient stop: %g, must be in the range [0,1]", stop );
        return;
    }

    if ( index >= m_stops.size() )
        m_stops.resize( index + 1 );

    m_stops[ index ].setPosition( stop );
}

qreal QskGradient::stopAt( int index ) const
{
    if ( index >= m_stops.size() )
        return -1.0;

    return m_stops[ index ].position();
}

void QskGradient::setColorAt( int index, const QColor& color )
{
    if ( !color.isValid() )
    {
        qWarning( "Invalid gradient color" );
        return;
    }

    if ( index >= m_stops.size() )
        m_stops.resize( index + 1 );

    m_stops[ index ].setColor( color );
}

QColor QskGradient::colorAt( int index ) const
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
}

bool QskGradient::hasStopAt( qreal value ) const
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

uint QskGradient::hash( uint seed ) const
{
    const int count = m_stops.size();
    if ( count == 0 )
        return seed;

    uint hash = qHashBits( &m_orientation, sizeof( m_orientation ), seed );
    for ( const auto& stop : m_stops )
        hash = stop.hash( hash );

    return hash;
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
    if ( from > to )
        return QskGradient( m_orientation );

    if ( isMonochrome() || ( from <= 0.0 && to >= 1.0 ) )
        return *this;

    from = qMax( from, 0.0 );
    to = qMin( to, 1.0 );

    const auto stops = qskExtractedStops( m_stops, from, to );
    return QskGradient( m_orientation, stops );
}

QskGradient QskGradient::interpolated(
    const QskGradient& to, qreal value ) const
{
    if ( !( isValid() && to.isValid() ) )
    {
        if ( !isValid() && !to.isValid() )
            return to;

        qreal progress;
        const QskGradient* gradient;

        if ( to.isValid() )
        {
            progress = value;
            gradient = &to;
        }
        else
        {
            progress = 1.0 - value;
            gradient = this;
        }

        /*
            We interpolate as if the invalid gradient would be
            a transparent version of the valid gradient
         */

        auto stops = gradient->m_stops;
        for ( auto& stop : stops )
        {
            auto c = stop.color();
            c.setAlpha( c.alpha() * progress );

            stop.setColor( c );
        }

        return QskGradient( gradient->m_orientation, stops );
    }

    if ( qskIsMonochrome( m_stops ) )
    {
        // we can ignore our stops

        const auto c = m_stops[ 0 ].color();

        auto s2 = to.m_stops;
        for ( int i = 0; i < s2.count(); i++ )
        {
            const auto c2 = QskRgb::interpolated( c, s2[ i ].color(), value );
            s2[ i ].setColor( c2 );
        }

        return QskGradient( to.m_orientation, s2 );
    }

    if ( qskIsMonochrome( to.m_stops ) )
    {
        // we can ignore the stops of to

        const auto c = to.m_stops[ 0 ].color();

        auto s2 = m_stops;
        for ( int i = 0; i < s2.count(); i++ )
        {
            const auto c2 = QskRgb::interpolated( s2[ i ].color(), c, value );
            s2[ i ].setColor( c2 );
        }

        return QskGradient( m_orientation, s2 );
    }

    if ( m_orientation == to.m_orientation )
    {
        /*
            we need to have the same number of stops
            at the same positions
         */

        const auto s1 = qskExpandedStops( m_stops, to.m_stops );
        auto s2 = qskExpandedStops( to.m_stops, m_stops );

        for ( int i = 0; i < s1.count(); i++ )
        {
            const auto c2 = QskRgb::interpolated(
                s1[ i ].color(), s2[ i ].color(), value );

            s2[ i ].setColor( c2 );
        }

        return QskGradient( m_orientation, s2 );
    }
    else
    {
        /*
            The interpolation is devided into 2 steps. First we
            interpolate into a monochrome gradient and then change
            the orientation before we continue in direction of the
            final gradient.
         */

        const auto c = m_stops[ 0 ].color();

        if ( value <= 0.5 )
        {
            auto s2 = m_stops;

            for ( int i = 0; i < s2.count(); i++ )
            {
                const auto c2 = QskRgb::interpolated(
                    s2[ i ].color(), c, 2 * value );

                s2[ i ].setColor( c2 );
            }

            return QskGradient( m_orientation, s2 );
        }
        else
        {
            auto s2 = to.m_stops;

            for ( int i = 0; i < s2.count(); i++ )
            {
                const auto c2 = QskRgb::interpolated(
                    c, s2[ i ].color(), 2 * ( value - 0.5 ) );

                s2[ i ].setColor( c2 );
            }

            return QskGradient( to.m_orientation, s2 );
        }
    }
}

QVariant QskGradient::interpolate(
    const QskGradient& from, const QskGradient& to, qreal progress )
{
    return QVariant::fromValue( from.interpolated( to, progress ) );
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskGradientStop& stop )
{
    debug << stop.position() << ": " << stop.color();
    return debug;
}

QDebug operator<<( QDebug debug, const QskGradient& gradient )
{
    debug << "GR:" << gradient.orientation() << gradient.stops().count();
    return debug;
}

#endif

#include "moc_QskGradient.cpp"
