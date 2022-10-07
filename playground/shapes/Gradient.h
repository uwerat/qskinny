/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the 3-clause BSD License
 *****************************************************************************/

#pragma once

/*
    These definitions will be somehow adjusted and merged with QskGradient
    and moved to the lib then.
 */

#include <QskControl.h>
#include <QskGradientStop.h>
#include <qbrush.h>
#include <qpoint.h>

class LinearGradient;
class RadialGradient;
class ConicGradient;

class Gradient
{
  public:
    constexpr Gradient() noexcept = default;
    Gradient( const Gradient& ) noexcept = default;

    bool operator==( const Gradient& ) const noexcept;
    bool operator!=( const Gradient& ) const noexcept;

    constexpr QGradient::Type type() const noexcept;

    constexpr bool isValid() const noexcept;

    bool isMonochrome() const;
    bool isVisible() const;

    void setStops( const QVector< QskGradientStop >& );
    const QVector< QskGradientStop >& stops() const noexcept;

    void setStops( QGradient::Preset );
    void setStops( const QGradientStops& );
    void setStops( const QColor&, const QColor& );
    QGradientStops qtStops() const;

    void setSpread( QGradient::Spread );
    constexpr QGradient::Spread spread() const noexcept;

    LinearGradient& asLinearGradient();
    const LinearGradient& asLinearGradient() const;

    RadialGradient& asRadialGradient();
    const RadialGradient& asRadialGradient() const;

    ConicGradient& asConicGradient();
    const ConicGradient& asConicGradient() const;

  private:
    friend class LinearGradient;
    friend class RadialGradient;
    friend class ConicGradient;

    constexpr Gradient( QGradient::Type ) noexcept;
    constexpr Gradient( QGradient::Type,
        qreal, qreal, qreal, qreal, qreal, qreal ) noexcept;

    QVector< QskGradientStop > m_stops;

    /*
        Linear: x1, y1, x2, y2, n/a, n/a
        Radial: centerX, centerY, centerRadius, focalX, focalY, focalRadius
        Conic: centerX, centerY, degrees, n/a, n/a, n/a
     */
    qreal m_values[6] = {};

    QGradient::Type m_type = QGradient::NoGradient;
    QGradient::Spread m_spread = QGradient::PadSpread;
};

class LinearGradient : public Gradient
{
  public:
    constexpr LinearGradient() noexcept;
    constexpr LinearGradient( const QPointF&, const QPointF& ) noexcept;
    constexpr LinearGradient( qreal x1, qreal y1, qreal x2, qreal y2 ) noexcept;

    void setStart(const QPointF& start) noexcept;
    void setStart( qreal x, qreal y ) noexcept;

    void setStop(const QPointF& start) noexcept;
    void setStop( qreal x, qreal y ) noexcept;

    constexpr QPointF start() const noexcept;
    constexpr QPointF stop() const noexcept;
};

#ifndef QT_NO_DEBUG_STREAM

class QDebug;
QDebug operator<<( QDebug, const Gradient& );

#endif

class RadialGradient : public Gradient
{
  public:
    constexpr RadialGradient() noexcept;

    constexpr RadialGradient( const QPointF& center, qreal radius ) noexcept;
    constexpr RadialGradient( qreal cx, qreal cy, qreal radius ) noexcept;

    constexpr RadialGradient( const QPointF& center, qreal centerRadius,
        const QPointF& focalPoint, qreal focalRadius = 0.0 ) noexcept;

    constexpr RadialGradient(qreal cx, qreal cy, qreal centerRadius,
        qreal fx, qreal fy, qreal focalRadius = 0.0 ) noexcept;

    constexpr QPointF center() const noexcept;
    void setCenter(const QPointF& center) noexcept;
    void setCenter(qreal x, qreal y) noexcept;

    constexpr QPointF focalPoint() const noexcept;
    void setFocalPoint( const QPointF& focalPoint ) noexcept;
    void setFocalPoint( qreal x, qreal y ) noexcept;

    constexpr qreal centerRadius() const noexcept;
    constexpr void setCenterRadius( qreal radius ) noexcept;

    constexpr qreal focalRadius() const noexcept;
    constexpr void setFocalRadius( qreal radius ) noexcept;
};

class ConicGradient : public Gradient
{
  public:
    constexpr ConicGradient() noexcept;
    constexpr ConicGradient( const QPointF&, qreal degrees ) noexcept;
    constexpr ConicGradient( qreal cx, qreal cy, qreal degrees ) noexcept;

    constexpr QPointF center() const noexcept;
    void setCenter(const QPointF& center) noexcept;
    void setCenter(qreal x, qreal y) noexcept;

    constexpr qreal degrees() const noexcept;
    void setDegrees(qreal ) noexcept;
};

constexpr Gradient::Gradient( QGradient::Type type ) noexcept
    : m_type( type )
{
}

constexpr Gradient::Gradient( QGradient::Type type,
        qreal v1, qreal v2, qreal v3,
        qreal v4 = 0.0, qreal v5 = 0.0, qreal v6 = 0.0  ) noexcept
    : m_values{ v1, v2, v3, v4, v5, v6 }
    , m_type( type )
{
}

inline bool Gradient::operator!=( const Gradient& other ) const noexcept
{
    return !( *this == other );
}

inline constexpr QGradient::Type Gradient::type() const noexcept
{
    return m_type;
}

constexpr bool Gradient::isValid() const noexcept
{
    return m_type != QGradient::NoGradient;
}

inline const QVector< QskGradientStop >& Gradient::stops() const noexcept
{
    return m_stops;
}

inline constexpr QGradient::Spread Gradient::spread() const noexcept
{
    return m_spread;
}

inline constexpr LinearGradient::LinearGradient() noexcept
    : Gradient( QGradient::LinearGradient, 0.0, 0.0, 1.0, 1.0 )
{
}

inline constexpr LinearGradient::LinearGradient(
        const QPointF& start, const QPointF& stop ) noexcept
    : LinearGradient( start.x(), start.y(), stop.x(), stop.y() )
{
}

inline constexpr LinearGradient::LinearGradient(
        qreal x1, qreal y1, qreal x2, qreal y2 ) noexcept
    : Gradient( QGradient::LinearGradient, x1, y1, x2, y2 )
{
}

inline constexpr QPointF LinearGradient::start() const noexcept
{
    return QPointF( m_values[0], m_values[1] );
}

inline constexpr QPointF LinearGradient::stop() const noexcept
{
    return QPointF( m_values[2], m_values[3] );
}

inline constexpr RadialGradient::RadialGradient() noexcept
    : Gradient( QGradient::RadialGradient, 0.0, 0.0, 1.0, 0.0, 0.0, 0.0 )
{
}

inline constexpr RadialGradient::RadialGradient(
        const QPointF& center, qreal radius,
        const QPointF& focalPoint, qreal focalRadius ) noexcept
    : RadialGradient( center.x(), center.y(), radius,
        focalPoint.x(), focalPoint.y(), focalRadius )
{
}

inline constexpr RadialGradient::RadialGradient(
        const QPointF& center, qreal radius ) noexcept
    : RadialGradient( center.x(), center.y(), radius )
{
}

inline constexpr RadialGradient::RadialGradient(
        qreal cx, qreal cy, qreal radius ) noexcept
    : RadialGradient( cx, cy, radius, cx, cy, 0.0 )
{
}

inline constexpr RadialGradient::RadialGradient(
        qreal cx, qreal cy, qreal radius,
        qreal fx, qreal fy, qreal focalRadius  ) noexcept
    : Gradient( QGradient::RadialGradient, cx, cy, radius, fx, fy, focalRadius )
{
}

inline constexpr QPointF RadialGradient::center() const noexcept
{
    return QPointF( m_values[0], m_values[1] );
}

inline constexpr qreal RadialGradient::centerRadius() const noexcept
{
    return m_values[2];
}

inline constexpr QPointF RadialGradient::focalPoint() const noexcept
{
    return QPointF( m_values[3], m_values[4] );
}

inline constexpr qreal RadialGradient::focalRadius() const noexcept
{
    return m_values[5];
}

inline constexpr ConicGradient::ConicGradient() noexcept
    : Gradient( QGradient::ConicalGradient, 0.0, 0.0, 0.0 )
{
}

inline constexpr ConicGradient::ConicGradient(
        const QPointF& center, qreal degrees ) noexcept
    : ConicGradient( center.x(), center.y(), degrees )
{
}

inline constexpr ConicGradient::ConicGradient(
        qreal cx, qreal cy, qreal degrees ) noexcept
    : Gradient( QGradient::ConicalGradient, cx, cy, degrees )
{
}

constexpr QPointF ConicGradient::center() const noexcept
{
    return QPointF( m_values[0], m_values[1] );
}

constexpr qreal ConicGradient::degrees() const noexcept
{
    return m_values[2];
}
