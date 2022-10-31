/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskConicGradient.h"

QskConicGradient::QskConicGradient( qreal cx, qreal cy, qreal startAngle,
        const QColor& color1, const QColor& color2 )
    : QskConicGradient( cx, cy, startAngle )
{
    setStops( color1, color2 );
}

QskConicGradient::QskConicGradient( qreal cx, qreal cy,
        qreal startAngle, QGradient::Preset preset )
    : QskConicGradient( cx, cy, startAngle )
{
    setStops( preset );
}

QskConicGradient::QskConicGradient( qreal cx, qreal cy,
        qreal startAngle, const QVector< QskGradientStop >& stops )
    : QskConicGradient( cx, cy, startAngle )
{
    setStops( stops );
}

inline void QskConicGradient::setCenter( const QPointF& center ) noexcept
{
    m_values[0] = center.x();
    m_values[1] = center.y();
}

inline void QskConicGradient::setCenter( qreal x, qreal y ) noexcept
{
    m_values[0] = x;
    m_values[1] = y;
}

inline void QskConicGradient::setStartAngle( qreal degrees ) noexcept
{
    m_values[2] = degrees;
}

inline void QskConicGradient::setSpanAngle( qreal degrees ) noexcept
{
    m_values[3] = qBound( -360.0, degrees, 360.0 );
}

#include "moc_QskConicGradient.cpp"

