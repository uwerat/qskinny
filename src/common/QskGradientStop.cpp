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

static inline QColor qskInterpolatedColor(
    const QskGradientStops& stops, int index1, int index2, qreal position )
{
    index1 = qBound( 0, index1, stops.count() - 1 );
    index2 = qBound( 0, index2, stops.count() - 1 );

    return QskGradientStop::interpolated( stops[ index1 ], stops[ index2 ], position );
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
        c.setAlpha( c.alpha() * ratio );

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

QskGradientStops qskExtractedGradientStops(
    const QskGradientStops& stops, qreal from, qreal to )
{
    if ( ( from > to ) || ( from > 1.0 ) || stops.isEmpty() )
        return QskGradientStops();

    if ( ( from <= 0.0 ) && ( to >= 1.0 ) )
        return stops;

    from = qMax( from, 0.0 );
    to = qMin( to, 1.0 );

    QVector< QskGradientStop > extracted;
    extracted.reserve( stops.count() );

    int i = 0;

    for ( ; i < stops.count(); i++ )
    {
        if ( stops[i].position() > from )
            break;
    }

    extracted += QskGradientStop( 0.0,
        qskInterpolatedColor( stops, i - 1, i, from ) );

    for ( ; i < stops.count(); i++ )
    {
        if ( stops[i].position() >= to )
            break;

        const auto pos = ( stops[i].position() - from ) / ( to - from );
        extracted += QskGradientStop( pos, stops[i].color() );
    }

    extracted += QskGradientStop( 1.0,
        qskInterpolatedColor( stops, i, i + 1, to ) );

    return extracted;
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
    QGradientStops qStops;
    qStops.reserve( stops.count() );

    for ( const auto& stop : stops )
    {
        QPair<qreal, QColor> qStop = { stop.position(), stop.color() };

        if ( !qStops.isEmpty() && qStops.last().first == qStop.first )
        {
            /*
                QGradient removes stops at the same position. So we have to insert
                an invisible dummy offset
             */
            qStop.first += 0.00001;
        }

        qStops += qStop;
    }

    return qStops;
}

