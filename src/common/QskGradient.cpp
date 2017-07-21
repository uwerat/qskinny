/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGradient.h"
#include <QDebug>

static bool isGradientValid( const QVector< QskGradientStop >& stops )
{
    if ( stops.size() < 2 )
        return false;

    if ( stops.first().position() != 0.0 ||
        stops.last().position() != 1.0 )
    {
        return false;
    }

    if ( !stops.first().color().isValid() )
        return false;

    for ( int i = 1; i < stops.size(); i++ )
    {
        if ( stops[i].position() <= stops[i - 1].position() )
            return false;

        if ( !stops[i].color().isValid() )
            return false;
    }

    return true;
}

QskGradientStop::QskGradientStop():
    m_position( -1.0 )
{
}

QskGradientStop::QskGradientStop( qreal position, const QColor& color ):
    m_position( position ),
    m_color( color )
{
}

qreal QskGradientStop::position() const
{
    return m_position;
}

void QskGradientStop::setPosition( qreal position )
{
    m_position = position;
}

void QskGradientStop::resetPosition()
{
    m_position = -1.0;
}

QColor QskGradientStop::color() const
{
    return m_color;
}

void QskGradientStop::setColor( const QColor& color )
{
    m_color = color;
}

void QskGradientStop::resetColor()
{
    m_color = QColor();
}

QskGradient::QskGradient():
    m_orientation( Vertical )
{
}

QskGradient::QskGradient( const QColor& color ):
    m_orientation( Vertical )
{
    setColor( color );
}

QskGradient::QskGradient( Orientation orientation,
        const QColor& startColor, const QColor& stopColor ):
    m_orientation( orientation )
{
    setColors( startColor, stopColor );
}

QskGradient::~QskGradient()
{
}

bool QskGradient::isValid() const
{
    return ::isGradientValid( m_stops );
}

void QskGradient::invalidate()
{
    m_stops.clear();
}

bool QskGradient::isMonochrome() const
{
    if ( !isValid() )
        return true;

    for ( int i = 1; i < m_stops.size(); i++ )
    {
        if ( m_stops[i] != m_stops[0] )
            return false;
    }

    return true;
}

bool QskGradient::isVisible() const
{
    return isValid() && ( m_stops[0].color().alpha() > 0 );
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
    setColors( color, color );
}

void QskGradient::setColors( const QColor& startColor, const QColor& stopColor )
{
    m_stops.resize( 2 );
    m_stops[0].setPosition( 0.0 );
    m_stops[0].setColor( startColor );
    m_stops[1].setPosition( 1.0 );
    m_stops[1].setColor( stopColor );
}

void QskGradient::setStops( const QVector< QskGradientStop >& stops )
{
    if ( !::isGradientValid( stops ) )
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

void QskGradient::setStopAt( int index, qreal stop )
{
    if ( stop < 0.0 || stop > 1.0 )
    {
        qWarning( "Invalid gradient stop: %g, must be in the range [0,1]", stop );
        return;
    }

    if ( index >= m_stops.size() )
        m_stops.resize( index + 1 );

    m_stops[index].setPosition( stop );
}

qreal QskGradient::stopAt( int index ) const
{
    if ( index >= m_stops.size() )
        return -1.0;

    return m_stops[index].position();
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

    m_stops[index].setColor( color );
}

QColor QskGradient::colorAt( int index ) const
{
    if ( index >= m_stops.size() )
        return QColor();

    return m_stops[index].color();
}

#ifndef QT_NO_DEBUG_STREAM

QDebug operator<<( QDebug debug, const QskGradientStop& stop )
{
    debug << "GR:" << stop.position() << stop.color();
    return debug;
}

QDebug operator<<( QDebug debug, const QskGradient& gradient )
{
    debug << "GR:" << gradient.orientation() << gradient.stops().count();
    return debug;
}

#endif

#include "moc_QskGradient.cpp"
