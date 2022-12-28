/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_GRDIENT_DIRECTION_H
#define QSK_GRDIENT_DIRECTION_H

#include "QskGlobal.h"

#include <qmetatype.h>
#include <qpoint.h>
#include <qline.h>

class QLineF;

class QSK_EXPORT QskLinearDirection
{
    Q_GADGET

    Q_PROPERTY( qreal x1 READ x1 WRITE setX1 )
    Q_PROPERTY( qreal y1 READ y1 WRITE setY1 )
    Q_PROPERTY( qreal x2 READ x2 WRITE setX2 )
    Q_PROPERTY( qreal y2 READ y2 WRITE setY2 )

  public:
    constexpr QskLinearDirection() noexcept = default;

    constexpr QskLinearDirection( Qt::Orientation ) noexcept;

    constexpr QskLinearDirection( const QLineF& ) noexcept;
    constexpr QskLinearDirection( const QPointF&, const QPointF& ) noexcept;
    constexpr QskLinearDirection( qreal x1, qreal y1, qreal x2, qreal y2 ) noexcept;

    void setVector( const QLineF& ) noexcept;
    void setVector( const QPointF&, const QPointF& ) noexcept;
    void setVector( qreal x1, qreal y1, qreal x2, qreal y2 ) noexcept;

    constexpr QLineF vector() const noexcept;

    void setStart( const QPointF& ) noexcept;
    void setStart( qreal x, qreal y ) noexcept;

    void setStop(const QPointF& ) noexcept;
    void setStop( qreal x, qreal y ) noexcept;

    void setInterval( Qt::Orientation, qreal, qreal );

    void setOrientation( Qt::Orientation ) noexcept;
    constexpr bool isOriented( Qt::Orientation ) const noexcept;

    constexpr bool isHorizontal() const noexcept;
    constexpr bool isVertical() const noexcept;
    constexpr bool isTilted() const noexcept;

    constexpr QPointF start() const noexcept;
    constexpr QPointF stop() const noexcept;

    constexpr qreal x1() const noexcept;
    void setX1( qreal ) noexcept;

    constexpr qreal y1() const noexcept;
    void setY1( qreal ) noexcept;

    constexpr qreal x2() const noexcept;
    void setX2( qreal ) noexcept;

    constexpr qreal y2() const noexcept;
    void setY2( qreal ) noexcept;

  private:
    qreal m_x1 = 0.0;
    qreal m_y1 = 0.0;
    qreal m_x2 = 0.0;
    qreal m_y2 = 1.0;
};

class QSK_EXPORT QskConicDirection
{
    Q_GADGET

    Q_PROPERTY( qreal x READ x WRITE setX )
    Q_PROPERTY( qreal y READ y WRITE setY )
    Q_PROPERTY( qreal startAngle READ startAngle WRITE setStartAngle )
    Q_PROPERTY( qreal spanAngle READ spanAngle WRITE setSpanAngle )

  public:
    // counter-clockwise
    constexpr QskConicDirection() noexcept = default;

    constexpr QskConicDirection( qreal x, qreal y, qreal startAngle = 0.0 ) noexcept;
    constexpr QskConicDirection( qreal x, qreal y, qreal startAngle, qreal spanAngle ) noexcept;

    constexpr QskConicDirection( const QPointF&, qreal startAngle = 0.0 ) noexcept;
    constexpr QskConicDirection( const QPointF&, qreal startAngle, qreal spanAngle ) noexcept;

    constexpr QPointF center() const noexcept;
    void setCenter(const QPointF& center) noexcept;
    void setCenter(qreal x, qreal y) noexcept;

    void setX( qreal ) noexcept;
    constexpr qreal x() const noexcept;

    void setY( qreal ) noexcept;
    constexpr qreal y() const noexcept;

    constexpr qreal startAngle() const noexcept;
    void setStartAngle( qreal ) noexcept;

    constexpr qreal spanAngle() const noexcept;
    void setSpanAngle( qreal ) noexcept;

  private:
    qreal m_x = 0.5;
    qreal m_y = 0.5;
    qreal m_startAngle = 0.0;
    qreal m_spanAngle = 360.0;
};

class QSK_EXPORT QskRadialDirection
{
    Q_GADGET

    Q_PROPERTY( qreal x READ x WRITE setX )
    Q_PROPERTY( qreal y READ y WRITE setY )
    Q_PROPERTY( qreal radiusX READ radiusX WRITE setRadiusX )
    Q_PROPERTY( qreal radiusY READ radiusY WRITE setRadiusY )

  public:
    constexpr QskRadialDirection() noexcept = default;

    constexpr QskRadialDirection( const QPointF& center, qreal radius ) noexcept;
    constexpr QskRadialDirection( qreal cx, qreal cy, qreal radius ) noexcept;
    constexpr QskRadialDirection( qreal cx, qreal cy, qreal radiusX, qreal radiusY ) noexcept;

    constexpr QPointF center() const noexcept;
    void setCenter(const QPointF& ) noexcept;
    void setCenter(qreal x, qreal y) noexcept;

    void setX( qreal ) noexcept;
    constexpr qreal x() const noexcept;

    void setY( qreal ) noexcept;
    constexpr qreal y() const noexcept;

    constexpr qreal radiusX() const noexcept;
    void setRadiusX( qreal ) noexcept;

    constexpr qreal radiusY() const noexcept;
    void setRadiusY( qreal ) noexcept;

    void setRadius( qreal ) noexcept;
    void setRadius( qreal, qreal ) noexcept;

  private:
    qreal m_x = 0.5;
    qreal m_y = 0.5;
    qreal m_radiusX = 0.5;
    qreal m_radiusY = 0.5;
};

inline constexpr QskLinearDirection::QskLinearDirection(
        Qt::Orientation orientation ) noexcept
    : m_x2( ( orientation == Qt::Vertical ) ? 0.0 : 1.0 )
    , m_y2( ( orientation == Qt::Vertical ) ? 1.0 : 0.0 )
{
}

inline constexpr QskLinearDirection::QskLinearDirection( const QLineF& vector ) noexcept
    : QskLinearDirection( vector.x1(), vector.y1(), vector.x2(), vector.y2() )
{
}

inline constexpr QskLinearDirection::QskLinearDirection(
        const QPointF& start, const QPointF& stop ) noexcept
    : QskLinearDirection( start.x(), start.y(), stop.x(), stop.y() )
{
}

inline constexpr QskLinearDirection::QskLinearDirection(
        qreal x1, qreal y1, qreal x2, qreal y2 ) noexcept
    : m_x1( x1 )
    , m_y1( y1 )
    , m_x2( x2 )
    , m_y2( y2 )
{
}

inline constexpr qreal QskLinearDirection::x1() const noexcept
{
    return m_x1;
}

inline constexpr qreal QskLinearDirection::y1() const noexcept
{
    return m_y1;
}

inline constexpr qreal QskLinearDirection::x2() const noexcept
{
    return m_x2;
}

inline constexpr qreal QskLinearDirection::y2() const noexcept
{
    return m_y2;
}

inline constexpr QLineF QskLinearDirection::vector() const noexcept
{
    return QLineF( m_x1, m_y1, m_x2, m_y2 );
}

inline constexpr QPointF QskLinearDirection::start() const noexcept
{
    return QPointF( m_x1, m_y1 );
}

inline constexpr QPointF QskLinearDirection::stop() const noexcept
{
    return QPointF( m_x2, m_y2 );
}

inline constexpr bool QskLinearDirection::isOriented(
    Qt::Orientation orientation ) const noexcept
{
    return ( orientation == Qt::Horizontal )
        ? isHorizontal() : isVertical();
}

inline constexpr bool QskLinearDirection::isHorizontal() const noexcept
{
    return !isVertical() && ( m_y1 == m_y2 );
}

inline constexpr bool QskLinearDirection::isVertical() const noexcept
{
    return m_x1 == m_x2;
}

inline constexpr bool QskLinearDirection::isTilted() const noexcept
{
    return ( m_x1 != m_x2 ) && ( m_y1 != m_y2 );
}

inline constexpr QskConicDirection::QskConicDirection(
        qreal x, qreal y, qreal startAngle ) noexcept
    : QskConicDirection( x, y, startAngle, 360.0 )
{
}

inline constexpr QskConicDirection::QskConicDirection(
        qreal x, qreal y, qreal startAngle, qreal spanAngle ) noexcept
    : m_x( x )
    , m_y( y )
    , m_startAngle( startAngle )
    , m_spanAngle( spanAngle )
{
}

inline constexpr QskConicDirection::QskConicDirection(
        const QPointF& pos, qreal startAngle ) noexcept
    : QskConicDirection( pos.x(), pos.y(), startAngle )
{
}

inline constexpr QskConicDirection::QskConicDirection(
        const QPointF& pos, qreal startAngle, qreal spanAngle ) noexcept
    : QskConicDirection( pos.x(), pos.y(), startAngle, spanAngle )
{
}

inline constexpr QPointF QskConicDirection::center() const noexcept
{
    return QPointF( m_x, m_y );
}

inline constexpr qreal QskConicDirection::startAngle() const noexcept
{
    return m_startAngle;
}

inline constexpr qreal QskConicDirection::spanAngle() const noexcept
{
    return m_spanAngle;
}

inline constexpr qreal QskConicDirection::x() const noexcept
{
    return m_x;
}

inline constexpr qreal QskConicDirection::y() const noexcept
{
    return m_y;
}

inline constexpr QskRadialDirection::QskRadialDirection(
        qreal x, qreal y, qreal radius ) noexcept
    : m_x( x )
    , m_y( y )
    , m_radiusX( radius )
    , m_radiusY( radius )
{
}

inline constexpr QskRadialDirection::QskRadialDirection(
        qreal x, qreal y, qreal radiusX, qreal radiusY ) noexcept
    : m_x( x )
    , m_y( y )
    , m_radiusX( radiusX )
    , m_radiusY( radiusY )
{
}

inline constexpr QskRadialDirection::QskRadialDirection(
        const QPointF& center, qreal radius ) noexcept
    : QskRadialDirection( center.x(), center.y(), radius )
{
}

inline constexpr QPointF QskRadialDirection::center() const noexcept
{
    return QPointF( m_x, m_y );
}

inline constexpr qreal QskRadialDirection::x() const noexcept
{
    return m_x;
}

inline constexpr qreal QskRadialDirection::y() const noexcept
{
    return m_y;
}

inline constexpr qreal QskRadialDirection::radiusX() const noexcept
{
    return m_radiusX;
}

inline constexpr qreal QskRadialDirection::radiusY() const noexcept
{
    return m_radiusY;
}

#endif
