/******************************************************************************
 * QSkinny - Copyright (C) The authors
 *           SPDX-License-Identifier: BSD-3-Clause
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

static inline qreal qskBoundedStopPos( qreal pos )
{
    if ( ( pos < 0.0 ) || qFuzzyIsNull( pos ) )
        return 0.0;

    if ( ( pos > 1.0 ) || qFuzzyCompare( pos, 1.0 ) )
        return 1.0;

    return pos;
}

static inline QVector< QskGradientStop >
qskNormalizedStops( const QVector< QskGradientStop >& stops )
{
    auto s = stops;

    if ( s.first().position() > 0.0 )
        s.prepend( QskGradientStop( 0.0, s.first().color() ) );

    if ( s.last().position() < 1.0 )
        s.append( QskGradientStop( 1.0, s.last().color() ) );

    return s;
}

void QskGradientStop::setPosition( qreal position ) noexcept
{
    m_position = position;
}

void QskGradientStop::setColor( const QColor& color ) noexcept
{
    m_color = color;
}

void QskGradientStop::setRgb( QRgb rgb ) noexcept
{
    m_color = QColor::fromRgba( rgb );
}

void QskGradientStop::setStop( qreal position, const QColor& color ) noexcept
{
    m_position = position;
    m_color = color;
}

void QskGradientStop::setStop( qreal position, Qt::GlobalColor color ) noexcept
{
    m_position = position;
    m_color = color;
}

QskHashValue QskGradientStop::hash( QskHashValue seed ) const noexcept
{
    auto hash = qHashBits( &m_position, sizeof( m_position ), seed );
    return qHashBits( &m_color, sizeof( m_color ), hash );
}

QskGradientStop QskGradientStop::interpolated(
    const QskGradientStop& to, qreal ratio ) const
{
    return QskGradientStop(
        m_position + ( to.m_position - m_position ) * ratio,
        QskRgb::interpolated( m_color, to.m_color, ratio )
    );
}

QVariant QskGradientStop::interpolate(
    const QskGradientStop& from, const QskGradientStop& to, qreal ratio )
{
    return QVariant::fromValue( from.interpolated( to, ratio ) );
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

// some helper functions around QskGradientStops

static inline QColor qskColorAtPosition(
    const QskGradientStop& s1, const QskGradientStop& s2, qreal pos )
{
    const auto dp = s2.position() - s1.position();
    if ( qFuzzyIsNull( dp ) )
        return s1.color();

    return QskRgb::interpolated(
        s1.color(), s2.color(), ( pos - s1.position() ) / dp );
}

static inline QskGradientStop qskCreateStopAtPosition(
    const QskGradientStop& s1, const QskGradientStop& s2, qreal pos )
{
    return { pos, qskColorAtPosition( s1, s2, pos ) };
}

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
        c.setAlpha( qRound( c.alpha() * ratio ) );

        stop.setColor( c );
    }

    return stops;
}

QskGradientStops qskInterpolatedGradientStops(
    const QskGradientStops& stops, const QColor& color, qreal ratio )
{
    auto newStops = stops;

    for ( auto& stop : newStops )
        stop.setColor( QskRgb::interpolated( stop.color(), color, ratio ) );

    return newStops;
}

QskGradientStops qskInterpolatedGradientStops(
    const QColor& color, const QskGradientStops& stops, qreal ratio )
{
    auto newStops = stops;

    for ( auto& stop : newStops )
        stop.setColor( QskRgb::interpolated( color, stop.color(), ratio ) );

    return newStops;
}

static QskGradientStops qskInterpolatedStops(
    const QskGradientStops& from, const QskGradientStops& to, qreal ratio )
{
    /*
        We have to insert interpolated stops for all positions
        that can be found in s1 and s2. So in case there is no
        stop at a specific position of the other stops we
        have to calculate one temporarily before interpolating.
     */
    QVector< QskGradientStop > stops;

    int i = 0, j = 0;
    while ( ( i < from.count() ) || ( j < to.count() ) )
    {
        qreal pos;
        QColor c1, c2;

        if ( i == from.count() )
        {
            c1 = from.last().color();
            c2 = to[j].color();
            pos = to[j++].position();
        }
        else if ( j == to.count() )
        {
            c1 = from[i].color();
            c2 = to.last().color();
            pos = from[i++].position();
        }
        else
        {
            c1 = from[i].color();
            c2 = to[j].color();

            const qreal dpos = from[i].position() - to[j].position();

            if ( qFuzzyIsNull( dpos ) )
            {
                pos = from[i++].position();
                j++;
            }
            else if ( dpos < 0.0 )
            {
                pos = from[i++].position();

                if ( j > 0 )
                {
                    const auto& stop = to[j - 1];
                    c2 = QskRgb::interpolated( stop.color(), c2, pos - stop.position() );
                }
            }
            else
            {
                pos = to[j++].position();

                if ( i > 0 )
                {
                    const auto& stop = from[i - 1];
                    c1 = QskRgb::interpolated( stop.color(), c1, pos - stop.position() );
                }
            }
        }

        stops += QskGradientStop( pos, QskRgb::interpolated( c1, c2, ratio ) );
    }

    return stops;
}

QskGradientStops qskInterpolatedGradientStops(
    const QskGradientStops& from, bool fromIsMonochrome,
    const QskGradientStops& to, bool toIsMonochrome, qreal ratio )
{
    if ( from.isEmpty() && to.isEmpty() )
        return QskGradientStops();

    if ( from.isEmpty() )
        return qskTransparentGradientStops( to, ratio );

    if ( to.isEmpty() )
        return qskTransparentGradientStops( from, 1.0 - ratio );

    if ( fromIsMonochrome && toIsMonochrome )
    {
        const auto c = QskRgb::interpolated(
            from[ 0 ].color(), to[ 0 ].color(), ratio );

        return { { 0.0, c }, { 1.0, c } };
    }

    if ( fromIsMonochrome )
    {
        return qskInterpolatedGradientStops( from[ 0 ].color(), to, ratio );
    }

    if ( toIsMonochrome )
    {
        return qskInterpolatedGradientStops( from, to[ 0 ].color(), ratio );
    }

    return qskInterpolatedStops( from, to, ratio );
}

QColor qskInterpolatedColorAt( const QskGradientStops& stops, qreal pos ) noexcept
{
    if ( stops.isEmpty() )
        return QColor();

    if ( pos <= stops.first().position() )
        return stops.first().color();

    for ( int i = 1; i < stops.count(); i++ )
    {
        if ( pos <= stops[ i ].position() )
            return qskColorAtPosition( stops[ i - 1 ], stops[ i ], pos );
    }

    return stops.last().color();
}

QskGradientStops qskReplacedGradientStops( const QskGradientStops& gradientStops,
    const QskGradientStop& stop1, const QskGradientStop& stop2 )
{
    if ( stop1.position() >= stop2.position() )
        return gradientStops;

    const auto s1 = QskGradientStop( qskBoundedStopPos( stop1.position() ), stop1.color() );
    const auto s2 = QskGradientStop( qskBoundedStopPos( stop2.position() ), stop2.color() );

    QskGradientStops stops;

    if ( s1.position() == 0.0 && s2.position() == 1.0 )
    {
        stops = { s1, s2 };
    }
    else if ( qskIsMonochrome( gradientStops ) )
    {
        stops.reserve( 4 );

        const auto c = gradientStops.isEmpty()
            ? QColor::fromRgba( 0 ) : gradientStops.first().color();

        if ( s1.position() > 0.0 )
            stops += { s1.position(), c };

        stops += s1;
        stops += s2;

        if ( s2.position() < 1.0 )
            stops += { s2.position(), c };
    }
    else
    {
        // not the most efficient implementation - maybe later TODO ...
        const auto stops0 = qskNormalizedStops( gradientStops );

        int i = 0;

        if ( s1.position() > 0.0 )
        {
            while ( s1.position() > stops0[i].position() )
                stops += stops0[i++];

            if ( s1.position() == stops0[i].position() )
                stops += stops0[i++];
            else
                stops += qskCreateStopAtPosition( stops0[i - 1], stops0[i], s1.position() );
        }

        stops += s1;

        while ( s2.position() > stops0[i].position() )
            i++;

        stops += s2;

        if ( s2.position() < 1.0 )
        {
            while ( stops0[i + 1].position() == s2.position() )
                i++;

            if ( s2.position() != stops0[i].position() )
                stops += qskCreateStopAtPosition( stops0[i - 1], stops0[i], s2.position() );

            while( i < stops0.count() )
                stops += stops0[i++];
        }
    }

    return stops;
}

QskGradientStops qskClippedGradientStops(
    const QskGradientStops& stops, qreal from, qreal to )
{
    return qskReplacedGradientStops( stops, { from, 0 }, { to, 0 } );
}

QskGradientStops qskExtractedGradientStops(
    const QskGradientStops& stops, qreal from, qreal to )
{
    if ( ( from > to ) || ( to > 1.0 ) || ( from < 0.0 ) || stops.isEmpty() )
        return QskGradientStops();

    from = qskBoundedStopPos( from );
    to = qskBoundedStopPos( to );

    if ( ( from == 0.0 ) && ( to == 1.0 ) )
        return stops;

    if ( from == to )
    {
        const auto color = qskInterpolatedColorAt( stops, from );

        QVector< QskGradientStop > s;
        s.reserve( 2 );
        s += QskGradientStop( 0.0, color );
        s += QskGradientStop( 1.0, color );

        return s;
    }

    /*
        For situations where we have no stops at 0.0 and 1.0 we insert them
        manually. Not the most efficient implementation, but we avoid having
        to deal with these situations for the moment. TODO ...
     */
    const auto stops1 = qskNormalizedStops( stops );

    QVector< QskGradientStop > stops2;
    stops2.reserve( stops1.count() );

    if ( stops1.count() == 2 )
    {
        const auto rgb1 = stops1.first().rgb();
        const auto rgb2 = stops1.last().rgb();

        stops2 += QskGradientStop( 0.0, QskRgb::interpolated( rgb1, rgb2, from ) );
        stops2 += QskGradientStop( 1.0, QskRgb::interpolated( rgb1, rgb2, to ) );
    }
    else
    {
        int i = 0;

        if ( from == 0.0 )
        {
            stops2 += stops1[i++];
        }
        else
        {
            while( stops1[++i].position() <= from ); // skip leading stops

            stops2 += QskGradientStop( 0.0,
                qskColorAtPosition( stops1[i - 1], stops1[ i ], from ) );
        }

        while ( stops1[i].position() < to )
        {
            const auto pos = ( stops1[i].position() - from ) / ( to - from );
            stops2 += QskGradientStop( pos, stops1[i++].color() );
        }

        stops2 += QskGradientStop( 1.0,
            qskColorAtPosition( stops1[i - 1], stops1[ i ], to ) );
    }

    return stops2;
}

QskGradientStops qskRevertedGradientStops( const QskGradientStops& stops )
{
    QVector< QskGradientStop > s;
    s.reserve( stops.count() );

    for ( auto it = stops.crbegin(); it != stops.crend(); ++it )
        s += QskGradientStop( 1.0 - it->position(), it->color() );

    return s;
}

QVector< QskGradientStop > qskFromQGradientStops( const QGradientStops& qtStops )
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
    QGradientStops qStops;
    qStops.reserve( stops.count() );

    for ( const auto& stop : stops )
    {
        QPair< qreal, QColor > qStop = { stop.position(), stop.color() };

        if ( !qStops.isEmpty() && qStops.last().first == qStop.first )
        {
            /*
                QGradient removes stops at the same position. So we have to insert
                an invisible dummy offset
             */
            qStop.first = qMin( qStop.first + 0.00001, 1.0 );
        }

        qStops += qStop;
    }

    return qStops;
}

#include "moc_QskGradientStop.cpp"
