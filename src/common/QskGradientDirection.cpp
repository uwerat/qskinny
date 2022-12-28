/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskGradientDirection.h"

static void qskRegisterGradientDirection()
{
    qRegisterMetaType< QskLinearDirection >();
    qRegisterMetaType< QskConicDirection >();
    qRegisterMetaType< QskRadialDirection >();
}

Q_CONSTRUCTOR_FUNCTION( qskRegisterGradientDirection )

// -- QskLinearDirection

void QskLinearDirection::setOrientation( Qt::Orientation orientation  ) noexcept
{
    m_x1 = m_y1 = 0.0;
    setStart( 0.0, 0.0 );

    if ( orientation == Qt::Vertical )
    {
        m_x2 = 0.0;
        m_y2 = 1.0;
    }
    else
    {
        m_x2 = 1.0;
        m_y2 = 0.0;
    }
}

void QskLinearDirection::setVector( const QLineF& vector ) noexcept
{
    m_x1 = vector.x1();
    m_y1 = vector.y1();
    m_x2 = vector.x2();
    m_y2 = vector.y2();
}

void QskLinearDirection::setVector( const QPointF& start, const QPointF& stop ) noexcept
{
    m_x1 = start.x();
    m_y1 = start.y();
    m_x2 = stop.x();
    m_y2 = stop.y();
}

void QskLinearDirection::setVector( qreal x1, qreal y1, qreal x2, qreal y2 ) noexcept
{
    m_x1 = x1;
    m_y1 = y1;
    m_x2 = x2;
    m_y2 = y2;
}

void QskLinearDirection::setStart( const QPointF& pos ) noexcept
{
    m_x1 = pos.x();
    m_y1 = pos.y();
}

void QskLinearDirection::setStart( qreal x, qreal y ) noexcept
{
    m_x1 = x;
    m_y1 = y;
}

void QskLinearDirection::setStop( const QPointF& pos ) noexcept
{
    m_x2 = pos.x();
    m_y2 = pos.y();
}

void QskLinearDirection::setStop( qreal x, qreal y ) noexcept
{
    m_x2 = x;
    m_y2 = y;
}

void QskLinearDirection::setX1( qreal x ) noexcept
{
    m_x1 = x;
}

void QskLinearDirection::setY1( qreal y ) noexcept
{
    m_y1 = y;
}

void QskLinearDirection::setX2( qreal x ) noexcept
{
    m_x2 = x;
}

void QskLinearDirection::setY2( qreal y ) noexcept
{
    m_y2 = y;
}

void QskLinearDirection::setInterval( Qt::Orientation orientation, qreal from, qreal to )
{
    if ( orientation == Qt::Vertical )
    {
        m_y1 = from;
        m_y2 = to;
    }
    else
    {
        m_x1 = from;
        m_x2 = to;
    }
}

// -- QskConicDirection

void QskConicDirection::setCenter( const QPointF& center ) noexcept
{
    m_x = center.x();
    m_y = center.y();
}

void QskConicDirection::setCenter( qreal x, qreal y ) noexcept
{
    m_x = x;
    m_y = y;
}

void QskConicDirection::setX( qreal x ) noexcept
{
    m_x = x;
}

void QskConicDirection::setY( qreal y ) noexcept
{
    m_y = y;
}

void QskConicDirection::setStartAngle( qreal degrees ) noexcept
{
    m_startAngle = degrees;
}

void QskConicDirection::setSpanAngle( qreal degrees ) noexcept
{
    m_spanAngle = qBound( -360.0, degrees, 360.0 );
}

// -- QskRadialDirection

void QskRadialDirection::setCenter( const QPointF& center ) noexcept
{
    m_x = center.x();
    m_y = center.y();
}

void QskRadialDirection::setCenter( qreal x, qreal y ) noexcept
{
    m_x = x;
    m_y = y;
}

void QskRadialDirection::setX( qreal x ) noexcept
{
    m_x = x;
}

void QskRadialDirection::setY( qreal y ) noexcept
{
    m_y = y;
}

void QskRadialDirection::setRadiusX( qreal radius ) noexcept
{
    m_radiusX = radius;
}

void QskRadialDirection::setRadiusY( qreal radius ) noexcept
{
    m_radiusY = radius;
}

void QskRadialDirection::setRadius( qreal radius ) noexcept
{
    m_radiusX = m_radiusY = radius;
}

void QskRadialDirection::setRadius( qreal radiusX, qreal radiusY ) noexcept
{
    m_radiusX = radiusX;
    m_radiusY = radiusY;
}

#include "moc_QskGradientDirection.cpp"
