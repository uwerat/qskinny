/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_RADIAL_GRADIENT_H
#define QSK_RADIAL_GRADIENT_H

#include "QskGradient.h"

class QSK_EXPORT QskRadialGradient : public QskGradient
{
    Q_GADGET

    Q_PROPERTY( QPointF center READ center WRITE setCenter )
    Q_PROPERTY( qreal radius READ radius WRITE setRadius )

  public:
    QskRadialGradient() noexcept;

    QskRadialGradient( const QColor&, const QColor& );
    QskRadialGradient( QGradient::Preset );
    QskRadialGradient( const QVector< QskGradientStop >& );

    QskRadialGradient( const QPointF& center, qreal radius ) noexcept;
    QskRadialGradient( qreal cx, qreal cy, qreal radius ) noexcept;

    QskRadialGradient( const QPointF& center, qreal radius,
        const QColor&, const QColor& );

    QskRadialGradient( qreal cx, qreal cy, qreal radius,
        const QColor&, const QColor& );

    QskRadialGradient( const QPointF& center, qreal radius, QGradient::Preset );
    QskRadialGradient( qreal cx, qreal cy, qreal radius, QGradient::Preset );

    QskRadialGradient( const QPointF& center, qreal radius,
        const QVector< QskGradientStop >& );

    QskRadialGradient( qreal cx, qreal cy, qreal radius,
        const QVector< QskGradientStop >& );

    QPointF center() const noexcept;
    void setCenter(const QPointF& center) noexcept;
    void setCenter(qreal x, qreal y) noexcept;

    qreal radius() const noexcept;
    void setRadius( qreal radius ) noexcept;
};

inline QskRadialGradient::QskRadialGradient() noexcept
    : QskGradient( Radial, 0.5, 0.5, 0.5, 0.0 )
{
}

inline QskRadialGradient::QskRadialGradient(
        const QPointF& center, qreal radius ) noexcept
    : QskRadialGradient( center.x(), center.y(), radius )
{
}

inline QskRadialGradient::QskRadialGradient(
        const QPointF& center, qreal radius, const QColor& start, const QColor& stop )
    : QskRadialGradient( center.x(), center.y(), radius, start, stop )
{
}

inline QskRadialGradient::QskRadialGradient(
        const QPointF& center, qreal radius, QGradient::Preset preset )
    : QskRadialGradient( center.x(), center.y(), radius, preset )
{
}

inline QskRadialGradient::QskRadialGradient(
        const QPointF& center, qreal radius, const QVector< QskGradientStop >& stops )
    : QskRadialGradient( center.x(), center.y(), radius, stops )
{
}

inline QPointF QskRadialGradient::center() const noexcept
{
    return QPointF( m_values[0], m_values[1] );
}

inline qreal QskRadialGradient::radius() const noexcept
{
    return m_values[2];
}

#endif
