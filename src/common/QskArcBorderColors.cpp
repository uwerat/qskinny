/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskArcBorderColors.h"
#include "QskRgbValue.h"

#include <qhashfunctions.h>
#include <qvariant.h>

static void qskRegisterArcBorderColors()
{
    qRegisterMetaType< QskArcBorderColors >();

    QMetaType::registerConverter< QColor, QskArcBorderColors >(
        []( const QColor& color ) { return QskArcBorderColors( color ); } );
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterArcBorderColors )

static inline bool qskIsVisble( const QColor& c )
{
    return c.isValid() && ( c.alpha() > 0 );
}

static inline void qskSetColors( const QColor& c, QColor* colors )
{
    colors[ 0 ] = colors[ 1 ] = c.toRgb();
}

static inline void qskSetColors(
    const QColor& outer, const QColor& inner, QColor* colors )
{
    colors[ Qsk::Outer ] = outer.toRgb();
    colors[ Qsk::Inner ] = inner.toRgb();
}

QskArcBorderColors::QskArcBorderColors()
{
}

QskArcBorderColors::QskArcBorderColors(
    const QColor& outer, const QColor& inner )
{
    qskSetColors( outer, inner, m_colors );
}

QskArcBorderColors::QskArcBorderColors( const QColor& color )
{
    qskSetColors( color, m_colors );
}

QskArcBorderColors::~QskArcBorderColors()
{
}

bool QskArcBorderColors::operator==( const QskArcBorderColors& other ) const
{
    return ( m_colors[ 0 ] == other.m_colors[ 0 ] ) &&
           ( m_colors[ 1 ] == other.m_colors[ 1 ] );
}

void QskArcBorderColors::setAlpha( int alpha )
{
    for ( int i = 0; i < 2; i++ )
    {
        if ( m_colors[ i ].isValid() && m_colors[ i ].alpha() )
            m_colors[ i ].setAlpha( alpha );
    }
}

void QskArcBorderColors::setColors( const QColor& color )
{
    qskSetColors( color, m_colors );
}

void QskArcBorderColors::setColors(
    const QColor& outer, const QColor& inner )
{
    qskSetColors( outer, inner, m_colors );
}

void QskArcBorderColors::setColor(
    Qsk::ArcPosition position, const QColor& color )
{
    m_colors[ position ] = color.toRgb();
}

bool QskArcBorderColors::isVisible() const
{
    if ( qskIsVisble( m_colors[ 0 ] ) )
        return true;

    if ( qskIsVisble( m_colors[ 1 ] ) )
        return true;

    return false;
}

bool QskArcBorderColors::isMonochrome() const
{
    if ( m_colors[ 1 ] != m_colors[ 0 ] )
        return false;

    return true;
}

QskArcBorderColors QskArcBorderColors::interpolated(
    const QskArcBorderColors& to, qreal ratio ) const
{
    QskArcBorderColors colors;

    for ( size_t i = 0; i < 2; i++ )
    {
        colors.m_colors[ i ] = QskRgb::interpolated(
            m_colors[ i ], to.m_colors[ i ], ratio );
    }

    return colors;
}

QVariant QskArcBorderColors::interpolate(
    const QskArcBorderColors& from, const QskArcBorderColors& to, qreal ratio )
{
    return QVariant::fromValue( from.interpolated( to, ratio ) );
}

uint QskArcBorderColors::hash( uint seed ) const
{
    const QRgb rgb[] =
    {
        m_colors[ 0 ].rgba(),
        m_colors[ 1 ].rgba(),
    };

    return qHashBits( rgb, sizeof( rgb ), seed );
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

static inline void qskDebugColor( QDebug debug, const QColor& c )
{
    debug << '('
        << c.red() << ','
        << c.green() << ','
        << c.blue() << ','
        << c.alpha() << ')';
}

QDebug operator<<( QDebug debug, const QskArcBorderColors& colors )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "ArcBorderColors" << '(';

    debug << " outer";
    qskDebugColor( debug, colors.color( Qsk::Outer ) );

    debug << ", inner";
    qskDebugColor( debug, colors.color( Qsk::Inner ) );

    debug << " )";

    return debug;
}

#endif
