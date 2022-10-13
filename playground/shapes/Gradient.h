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
    Gradient() noexcept = default;
    Gradient( const Gradient& ) noexcept;

    Gradient& operator=( const Gradient& ) noexcept;

    bool operator==( const Gradient& ) const noexcept;
    bool operator!=( const Gradient& ) const noexcept;

    QGradient::Type type() const noexcept;

    bool isValid() const noexcept;

    bool isMonochrome() const;
    bool isVisible() const;

    void setStops( const QskGradientStops& );
    const QskGradientStops& stops() const noexcept;

    void setStops( QGradient::Preset );
    void setStops( const QGradientStops& );
    void setStops( const QColor&, const QColor& );
    QGradientStops qtStops() const;

    void setSpread( QGradient::Spread );
    QGradient::Spread spread() const noexcept;

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

    Gradient( QGradient::Type ) noexcept;
    Gradient( QGradient::Type, qreal, qreal, qreal, qreal ) noexcept;

    QskGradientStops m_stops;

    /*
        Linear: x1, y1, x2, y2
        Radial: centerX, centerY, radius, n/a
        Conic: centerX, centerY, startAngle, spanAngle
     */
    qreal m_values[4] = {};

    QGradient::Type m_type = QGradient::NoGradient;
    QGradient::Spread m_spread = QGradient::PadSpread;
};

#ifndef QT_NO_DEBUG_STREAM

class QDebug;
QDebug operator<<( QDebug, const QskGradient& );

#endif

class LinearGradient : public Gradient
{
  public:
    LinearGradient() noexcept;
    LinearGradient( const QPointF&, const QPointF& ) noexcept;
    LinearGradient( qreal x1, qreal y1, qreal x2, qreal y2 ) noexcept;

    void setStart(const QPointF& start) noexcept;
    void setStart( qreal x, qreal y ) noexcept;

    void setStop(const QPointF& start) noexcept;
    void setStop( qreal x, qreal y ) noexcept;

    QPointF start() const noexcept;
    QPointF stop() const noexcept;
};

class RadialGradient : public Gradient
{
  public:
    RadialGradient() noexcept;

    RadialGradient( const QPointF& center, qreal radius ) noexcept;
    RadialGradient( qreal cx, qreal cy, qreal radius ) noexcept;

    QPointF center() const noexcept;
    void setCenter(const QPointF& center) noexcept;
    void setCenter(qreal x, qreal y) noexcept;

    qreal radius() const noexcept;
    void setRadius( qreal radius ) noexcept;
};

class ConicGradient : public Gradient
{
  public:
    ConicGradient() noexcept;

    ConicGradient( const QPointF&,
        qreal startAngle = 0.0, qreal spanAngle = 360.0 ) noexcept;

    ConicGradient( qreal cx, qreal cy,
        qreal startAngle = 0.0, qreal spanAngle = 360.0 ) noexcept;

    QPointF center() const noexcept;
    void setCenter(const QPointF& center) noexcept;
    void setCenter(qreal x, qreal y) noexcept;

    qreal startAngle() const noexcept;
    void setStartAngle( qreal ) noexcept;

    qreal spanAngle() const noexcept;
    void setSpanAngle( qreal ) noexcept;
};

inline Gradient::Gradient( QGradient::Type type ) noexcept
    : m_type( type )
{
}

inline Gradient::Gradient( QGradient::Type type,
        qreal v1, qreal v2, qreal v3, qreal v4  ) noexcept
    : m_values{ v1, v2, v3, v4 }
    , m_type( type )
{
}

inline bool Gradient::operator!=( const Gradient& other ) const noexcept
{
    return !( *this == other );
}

inline QGradient::Type Gradient::type() const noexcept
{
    return m_type;
}

inline bool Gradient::isValid() const noexcept
{
    return m_type != QGradient::NoGradient;
}

inline const QskGradientStops& Gradient::stops() const noexcept
{
    return m_stops;
}

inline QGradient::Spread Gradient::spread() const noexcept
{
    return m_spread;
}

inline LinearGradient::LinearGradient() noexcept
    : Gradient( QGradient::LinearGradient, 0.0, 0.0, 1.0, 1.0 )
{
}

inline LinearGradient::LinearGradient(
        const QPointF& start, const QPointF& stop ) noexcept
    : LinearGradient( start.x(), start.y(), stop.x(), stop.y() )
{
}

inline LinearGradient::LinearGradient(
        qreal x1, qreal y1, qreal x2, qreal y2 ) noexcept
    : Gradient( QGradient::LinearGradient, x1, y1, x2, y2 )
{
}

inline QPointF LinearGradient::start() const noexcept
{
    return QPointF( m_values[0], m_values[1] );
}

inline QPointF LinearGradient::stop() const noexcept
{
    return QPointF( m_values[2], m_values[3] );
}

inline RadialGradient::RadialGradient() noexcept
    : Gradient( QGradient::RadialGradient, 0.5, 0.5, 0.5, 0.0 )
{
}

inline RadialGradient::RadialGradient(
        const QPointF& center, qreal radius ) noexcept
    : RadialGradient( center.x(), center.y(), radius )
{
}

inline RadialGradient::RadialGradient(
        qreal cx, qreal cy, qreal radius ) noexcept
    : Gradient( QGradient::RadialGradient, cx, cy, radius, 0.0 )
{
}

inline QPointF RadialGradient::center() const noexcept
{
    return QPointF( m_values[0], m_values[1] );
}

inline qreal RadialGradient::radius() const noexcept
{
    return m_values[2];
}

inline ConicGradient::ConicGradient() noexcept
    : Gradient( QGradient::ConicalGradient, 0.0, 0.0, 0.0, 0.0 )
{
}

inline ConicGradient::ConicGradient(
        const QPointF& center, qreal startAngle, qreal spanAngle ) noexcept
    : ConicGradient( center.x(), center.y(), startAngle, spanAngle )
{
}

inline ConicGradient::ConicGradient(
        qreal cx, qreal cy, qreal startAngle, qreal spanAngle ) noexcept
    : Gradient( QGradient::ConicalGradient, cx, cy, startAngle, spanAngle )
{
}

inline QPointF ConicGradient::center() const noexcept
{
    return QPointF( m_values[0], m_values[1] );
}

inline qreal ConicGradient::startAngle() const noexcept
{
    return m_values[2];
}

inline qreal ConicGradient::spanAngle() const noexcept
{
    return m_values[3];
}
