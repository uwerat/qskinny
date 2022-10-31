/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskRadialGradient.h"

QskRadialGradient::QskRadialGradient( qreal cx, qreal cy, qreal radius ) noexcept
    : QskGradient( Radial, cx, cy, radius, 0.0 )
{
}

QskRadialGradient::QskRadialGradient( const QColor& color1, const QColor& color2 )
    : QskRadialGradient()
{
    setStops( color1, color2 );
}

QskRadialGradient::QskRadialGradient( QGradient::Preset preset )
    : QskRadialGradient()
{
    setStops( preset );
}

QskRadialGradient::QskRadialGradient( const QVector< QskGradientStop >& stops )
    : QskRadialGradient()
{
    setStops( stops );
}

QskRadialGradient::QskRadialGradient( qreal cx, qreal cy, qreal radius,
        const QColor& color1, const QColor& color2 )
    : QskRadialGradient( cx, cy, radius )
{
    setStops( color1, color2 );
}

QskRadialGradient::QskRadialGradient( qreal cx, qreal cy,
        qreal radius, QGradient::Preset preset )
    : QskRadialGradient( cx, cy, radius )
{
    setStops( preset );
}

QskRadialGradient::QskRadialGradient( qreal cx, qreal cy,
        qreal radius, const QVector< QskGradientStop >& stops )
    : QskRadialGradient( cx, cy, radius )
{
    setStops( stops );
}

void QskRadialGradient::setCenter( const QPointF& center ) noexcept
{
    m_values[0] = center.x();
    m_values[1] = center.y();
}

void QskRadialGradient::setCenter( qreal x, qreal y ) noexcept
{
    m_values[0] = x;
    m_values[1] = y;
}

void QskRadialGradient::setRadius( qreal radius ) noexcept
{
    m_values[2] = radius;
}

#include "moc_QskRadialGradient.cpp"
