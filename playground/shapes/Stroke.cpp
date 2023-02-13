/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#include "Stroke.h"
#include <QPen>

Stroke::Stroke( const QPen& pen ) noexcept
    : m_width( pen.widthF() )
    , m_miterLimit( pen.miterLimit() )
    , m_color( pen.color() )
    , m_lineStyle( ( pen.style() == Qt::DashLine ) ? DashLine : SolidLine )
    , m_joinStyle( static_cast< JoinStyle >( pen.joinStyle() ) )
    , m_capStyle( static_cast< CapStyle >( pen.capStyle() ) )
    , m_cosmetic( pen.isCosmetic() )
{
}

void Stroke::setColor( const QColor& color ) noexcept
{
    m_color = color;
}

void Stroke::setWidth( qreal width ) noexcept
{
    m_width = width;
}

void Stroke::setLineStyle( LineStyle style )
{
    m_lineStyle = style;
}

void Stroke::setCapStyle( CapStyle capStyle ) noexcept
{
    m_capStyle = capStyle;
}

void Stroke::setJoinStyle( JoinStyle joinStyle ) noexcept
{
    m_joinStyle = joinStyle;
}

void Stroke::setMiterLimit( int miterLimit ) noexcept
{
    m_miterLimit = miterLimit;
}

void Stroke::setCosmetic( bool on ) noexcept
{
    m_cosmetic = on;
}

bool Stroke::isVisible() const
{
    return ( m_width > 0.0 ) && m_color.isValid() && ( m_color.alpha() > 0 );
}

QPen Stroke::toPen() const
{
    QPen pen(
        m_color,
        m_width,
        ( m_width > 0.0 ) ? static_cast< Qt::PenStyle >( m_lineStyle ) : Qt::NoPen,
        static_cast< Qt::PenCapStyle >( m_capStyle ),
        static_cast< Qt::PenJoinStyle >( m_joinStyle )
    );

    pen.setCosmetic( m_cosmetic );

    return pen;
}

#include "moc_Stroke.cpp"
