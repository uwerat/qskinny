/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskBoxBorderColors.h"
#include "QskRgbValue.h"

#include <qhashfunctions.h>
#include <qvariant.h>

static void qskRegisterBoxBorderColors()
{
    qRegisterMetaType< QskBoxBorderColors >();

    QMetaType::registerConverter< QColor, QskBoxBorderColors >(
        []( const QColor& color ) { return QskBoxBorderColors( color ); } );
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterBoxBorderColors )

static inline bool qskIsVisble( const QColor& c )
{
    return c.isValid() && ( c.alpha() > 0 );
}

static inline void qskSetColors( const QColor& c, QColor* colors )
{
    colors[ 0 ] = colors[ 1 ] = colors[ 2 ] = colors[ 3 ] = c.toRgb();
}

static inline void qskSetColors(
    const QColor& left, const QColor& top,
    const QColor& right, const QColor& bottom, QColor* colors )
{
    colors[ Qsk::Left ] = left.toRgb();
    colors[ Qsk::Top ] = top.toRgb();
    colors[ Qsk::Right ] = right.toRgb();
    colors[ Qsk::Bottom ] = bottom.toRgb();
}

QskBoxBorderColors::QskBoxBorderColors()
{
}

QskBoxBorderColors::QskBoxBorderColors(
    const QColor& left, const QColor& top,
    const QColor& right, const QColor& bottom )
{
    qskSetColors( left, top, right, bottom, m_colors );
}

QskBoxBorderColors::QskBoxBorderColors( const QColor& color )
{
    qskSetColors( color, m_colors );
}

QskBoxBorderColors::~QskBoxBorderColors()
{
}

bool QskBoxBorderColors::operator==( const QskBoxBorderColors& other ) const
{
    return ( m_colors[ 0 ] == other.m_colors[ 0 ] ) &&
           ( m_colors[ 1 ] == other.m_colors[ 1 ] ) &&
           ( m_colors[ 2 ] == other.m_colors[ 2 ] ) &&
           ( m_colors[ 3 ] == other.m_colors[ 3 ] );
}

void QskBoxBorderColors::setAlpha( int alpha )
{
    for ( int i = 0; i < 4; i++ )
    {
        if ( m_colors[ i ].isValid() && m_colors[ i ].alpha() )
            m_colors[ i ].setAlpha( alpha );
    }
}

void QskBoxBorderColors::setColors( const QColor& color )
{
    qskSetColors( color, m_colors );
}

void QskBoxBorderColors::setColors(
    const QColor& left, const QColor& top,
    const QColor& right, const QColor& bottom )
{
    qskSetColors( left, top, right, bottom, m_colors );
}

void QskBoxBorderColors::setColor(
    Qsk::Position position, const QColor& color )
{
    m_colors[ position ] = color.toRgb();
}

void QskBoxBorderColors::setColorsAt( Qt::Edges edges, const QColor& color )
{
    const QColor c = color.toRgb();

    if ( edges & Qt::TopEdge )
        m_colors[ Qsk::Top ] = c;

    if ( edges & Qt::LeftEdge )
        m_colors[ Qsk::Left ] = c;

    if ( edges & Qt::RightEdge )
        m_colors[ Qsk::Right ] = c;

    if ( edges & Qt::BottomEdge )
        m_colors[ Qsk::Bottom ] = c;
}

QColor QskBoxBorderColors::colorAt( Qt::Edge edge ) const
{
    switch ( edge )
    {
        case Qt::TopEdge:
            return m_colors[ Qsk::Top ];

        case Qt::LeftEdge:
            return m_colors[ Qsk::Left ];

        case Qt::RightEdge:
            return m_colors[ Qsk::Right ];

        case Qt::BottomEdge:
            return m_colors[ Qsk::Bottom ];
    }

    return QColor();
}

bool QskBoxBorderColors::isVisible() const
{
    if ( qskIsVisble( m_colors[ 0 ] ) )
        return true;

    if ( qskIsVisble( m_colors[ 1 ] ) )
        return true;

    if ( qskIsVisble( m_colors[ 2 ] ) )
        return true;

    if ( qskIsVisble( m_colors[ 3 ] ) )
        return true;

    return false;
}

bool QskBoxBorderColors::isMonochrome() const
{
    if ( m_colors[ 1 ] != m_colors[ 0 ] )
        return false;

    if ( m_colors[ 2 ] != m_colors[ 1 ] )
        return false;

    if ( m_colors[ 3 ] != m_colors[ 2 ] )
        return false;

    return true;
}

QskBoxBorderColors QskBoxBorderColors::interpolated(
    const QskBoxBorderColors& to, qreal ratio ) const
{
    QskBoxBorderColors colors;

    for ( size_t i = 0; i < 4; i++ )
    {
        colors.m_colors[ i ] = QskRgb::interpolated(
            m_colors[ i ], to.m_colors[ i ], ratio );
    }

    return colors;
}

QVariant QskBoxBorderColors::interpolate(
    const QskBoxBorderColors& from, const QskBoxBorderColors& to, qreal ratio )
{
    return QVariant::fromValue( from.interpolated( to, ratio ) );
}

uint QskBoxBorderColors::hash( uint seed ) const
{
    const QRgb rgb[] =
    {
        m_colors[ 0 ].rgba(),
        m_colors[ 1 ].rgba(),
        m_colors[ 2 ].rgba(),
        m_colors[ 3 ].rgba(),
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

QDebug operator<<( QDebug debug, const QskBoxBorderColors& colors )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "BoxBorderColors" << '(';

    debug << " L";
    qskDebugColor( debug, colors.color( Qsk::Left ) );

    debug << ", T";
    qskDebugColor( debug, colors.color( Qsk::Top ) );

    debug << ", R";
    qskDebugColor( debug, colors.color( Qsk::Right ) );

    debug << ", B";
    qskDebugColor( debug, colors.color( Qsk::Bottom ) );

    debug << " )";

    return debug;
}

#endif
