/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 *           SPDX-License-Identifier: BSD-3-Clause
 *****************************************************************************/

#include "QskGradientDirection.h"
#include <qrect.h>

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
    m_dot = -1.0;
}

void QskLinearDirection::setVector( const QPointF& start, const QPointF& stop ) noexcept
{
    m_x1 = start.x();
    m_y1 = start.y();
    m_x2 = stop.x();
    m_y2 = stop.y();
    m_dot = -1.0;
}

void QskLinearDirection::setVector( qreal x1, qreal y1, qreal x2, qreal y2 ) noexcept
{
    m_x1 = x1;
    m_y1 = y1;
    m_x2 = x2;
    m_y2 = y2;
    m_dot = -1.0;
}

void QskLinearDirection::setStart( const QPointF& pos ) noexcept
{
    m_x1 = pos.x();
    m_y1 = pos.y();
    m_dot = -1.0;
}

void QskLinearDirection::setStart( qreal x, qreal y ) noexcept
{
    m_x1 = x;
    m_y1 = y;
    m_dot = -1.0;
}

void QskLinearDirection::setStop( const QPointF& pos ) noexcept
{
    m_x2 = pos.x();
    m_y2 = pos.y();
    m_dot = -1.0;
}

void QskLinearDirection::setStop( qreal x, qreal y ) noexcept
{
    m_x2 = x;
    m_y2 = y;
    m_dot = -1.0;
}

void QskLinearDirection::setX1( qreal x ) noexcept
{
    m_x1 = x;
    m_dot = -1.0;
}

void QskLinearDirection::setY1( qreal y ) noexcept
{
    m_y1 = y;
    m_dot = -1.0;
}

void QskLinearDirection::setX2( qreal x ) noexcept
{
    m_x2 = x;
    m_dot = -1.0;
}

void QskLinearDirection::setY2( qreal y ) noexcept
{
    m_y2 = y;
    m_dot = -1.0;
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
    m_dot = -1.0;
}

void QskLinearDirection::precalculate() const noexcept
{
    m_dx = m_x2 - m_x1;
    m_dy = m_y2 - m_y1;
    m_dot = m_dx * m_dx + m_dy * m_dy;
}

static inline bool qskIntersectsTop(
    qreal vx, qreal vy, qreal m, const QRectF& rect )
{
    const auto cx = vx - ( vy - rect.top() ) / m;
    return cx > rect.left() && cx < rect.right();
}

static inline bool qskIntersectsBottom(
    qreal vx, qreal vy, qreal m, const QRectF& rect )
{
    const auto cx = vx - ( vy - rect.bottom() ) / m;
    return cx > rect.left() && cx < rect.right();
}

static inline bool qskIntersectsLeft(
    qreal vx, qreal vy, qreal m, const QRectF& rect )
{
    const auto cy = vy - ( vx - rect.left() ) * m;
    return ( cy > rect.top() && cy < rect.bottom() );
}

static inline bool qskIntersectsRight(
    qreal vx, qreal vy, qreal m, const QRectF& rect )
{
    const auto cy = vy - ( vx - rect.right() ) * m;
    return ( cy > rect.top() && cy < rect.bottom() );
}

bool QskLinearDirection::contains( const QRectF& rect ) const
{
    if ( m_y1 == m_y2 )
    {
        return ( m_x1 <= rect.left() && m_x2 >= rect.right() )
            || ( m_x1 >= rect.right() && m_x2 <= rect.left() );
    }

    if ( m_x1 == m_x2 )
    {
        return ( m_y1 <= rect.top() && m_y2 >= rect.bottom() )
            || ( m_y1 >= rect.bottom() && m_y2 <= rect.top() );
    }

    // are the normal vectors intersecting ?

    const auto m = ( m_x2 - m_x1 ) / ( m_y1 - m_y2 ); // slope of the normal vectors

    const bool intersecting =
        qskIntersectsLeft( m_x1, m_y1, m, rect ) ||
        qskIntersectsRight( m_x1, m_y1, m, rect ) ||
        qskIntersectsTop( m_x1, m_y1, m, rect ) ||
        qskIntersectsBottom( m_x1, m_y1, m, rect ) ||
        qskIntersectsLeft( m_x2, m_y2, m, rect ) ||
        qskIntersectsRight( m_x2, m_y2, m, rect ) ||
        qskIntersectsTop( m_x2, m_y2, m, rect ) ||
        qskIntersectsBottom( m_x2, m_y2, m, rect );

    return !intersecting;
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
