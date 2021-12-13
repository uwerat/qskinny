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

static inline void qskSetColors( const QColor& c, QskGradient* colors )
{
    colors[ 0 ] = colors[ 1 ] = colors[ 2 ] = colors[ 3 ] = c.toRgb();
}

static inline void qskSetColors(
    const QColor& left, const QColor& top,
    const QColor& right, const QColor& bottom, QskGradient* colors )
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
    qskSetColors( left, top, right, bottom, m_gradients );
}

QskBoxBorderColors::QskBoxBorderColors( const QColor& color )
{
    qskSetColors( color, m_gradients );
}

QskBoxBorderColors::~QskBoxBorderColors()
{
}

bool QskBoxBorderColors::operator==( const QskBoxBorderColors& other ) const
{
    return ( m_gradients[ 0 ] == other.m_gradients[ 0 ] ) &&
           ( m_gradients[ 1 ] == other.m_gradients[ 1 ] ) &&
           ( m_gradients[ 2 ] == other.m_gradients[ 2 ] ) &&
           ( m_gradients[ 3 ] == other.m_gradients[ 3 ] );
}

void QskBoxBorderColors::setAlpha( int alpha )
{
    for ( int i = 0; i < 4; i++ )
    {
        if ( m_gradients[ i ].isValid() )
            m_gradients[ i ].setAlpha( alpha );
    }
}

void QskBoxBorderColors::setColors( const QColor& color )
{
    qskSetColors( color, m_gradients );
}

void QskBoxBorderColors::setColors(
    const QColor& left, const QColor& top,
    const QColor& right, const QColor& bottom )
{
    qskSetColors( left, top, right, bottom, m_gradients );
}

void QskBoxBorderColors::setColor(
    Qsk::Position position, const QColor& color )
{
    m_gradients[ position ] = color.toRgb();
}

void QskBoxBorderColors::setColorsAt( Qt::Edges edges, const QColor& color )
{
    const QColor c = color.toRgb();

    if ( edges & Qt::TopEdge )
        m_gradients[ Qsk::Top ] = c;

    if ( edges & Qt::LeftEdge )
        m_gradients[ Qsk::Left ] = c;

    if ( edges & Qt::RightEdge )
        m_gradients[ Qsk::Right ] = c;

    if ( edges & Qt::BottomEdge )
        m_gradients[ Qsk::Bottom ] = c;
}

QskGradient QskBoxBorderColors::colorAt( Qt::Edge edge ) const
{
    switch ( edge )
    {
        case Qt::TopEdge:
            return m_gradients[ Qsk::Top ];

        case Qt::LeftEdge:
            return m_gradients[ Qsk::Left ];

        case Qt::RightEdge:
            return m_gradients[ Qsk::Right ];

        case Qt::BottomEdge:
            return m_gradients[ Qsk::Bottom ];
    }

    return QColor();
}

bool QskBoxBorderColors::isVisible() const
{
    if ( m_gradients[ 0 ].isVisible() )
        return true;

    if ( m_gradients[ 1 ].isVisible() )
        return true;

    if ( m_gradients[ 2 ].isVisible() )
        return true;

    if ( m_gradients[ 3 ].isVisible() )
        return true;

    return false;
}

bool QskBoxBorderColors::isMonochrome() const
{
    if ( m_gradients[ 1 ] != m_gradients[ 0 ] )
        return false;

    if ( m_gradients[ 2 ] != m_gradients[ 1 ] )
        return false;

    if ( m_gradients[ 3 ] != m_gradients[ 2 ] )
        return false;

    return m_gradients[ 0 ].isMonochrome()
        && m_gradients[ 1 ].isMonochrome()
        && m_gradients[ 2 ].isMonochrome()
        && m_gradients[ 3 ].isMonochrome();
}

QskBoxBorderColors QskBoxBorderColors::interpolated(
    const QskBoxBorderColors& to, qreal ratio ) const
{
    QskBoxBorderColors colors;

    for ( size_t i = 0; i < 4; i++ )
    {
        colors.m_gradients[ i ] = colors.m_gradients[ i ].interpolated(
            to.m_gradients[ i ], ratio );
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
    uint h = m_gradients[ 0 ].hash( seed );
    h = m_gradients[ 1 ].hash( h );
    h = m_gradients[ 2 ].hash( h );
    h = m_gradients[ 3 ].hash( h );

    return h;
}

#ifndef QT_NO_DEBUG_STREAM

#include <qdebug.h>

QDebug operator<<( QDebug debug, const QskBoxBorderColors& colors )
{
    QDebugStateSaver saver( debug );
    debug.nospace();

    debug << "BoxBorderColors" << '(';

    debug << " L" << colors.color( Qsk::Left );

    debug << ", T" << colors.color( Qsk::Top );

    debug << ", R" << colors.color( Qsk::Right );

    debug << ", B" << colors.color( Qsk::Bottom );

    debug << " )";

    return debug;
}

#endif
