/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_CONIC_GRADIENT_H
#define QSK_CONIC_GRADIENT_H

#include "QskGradient.h"

class QSK_EXPORT QskConicGradient : public QskGradient
{
    Q_GADGET

    Q_PROPERTY( QPointF center READ center WRITE setCenter )
    Q_PROPERTY( qreal startAngle READ startAngle WRITE setStartAngle )
    Q_PROPERTY( qreal spanAngle READ spanAngle WRITE setSpanAngle )

  public:
    // counter-clockwise
    QskConicGradient() noexcept;

    QskConicGradient( const QColor&, const QColor& );
    QskConicGradient( QGradient::Preset );
    QskConicGradient( const QVector< QskGradientStop >& );

    QskConicGradient( const QPointF&,
        qreal startAngle = 0.0, qreal spanAngle = 360.0 ) noexcept;

    QskConicGradient( qreal cx, qreal cy,
        qreal startAngle = 0.0, qreal spanAngle = 360.0 ) noexcept;

    QskConicGradient( qreal cx, qreal cy, qreal startAngle,
        const QColor&, const QColor& );

    QskConicGradient( const QPointF&, qreal startAngle,
        const QColor&, const QColor& );

    QskConicGradient( qreal cx, qreal cy, qreal startAngle, QGradient::Preset );

    QskConicGradient( const QPointF&, qreal startAngle, QGradient::Preset );

    QskConicGradient( qreal cx, qreal cy, qreal startAngle,
        const QVector< QskGradientStop >& );

    QskConicGradient( const QPointF&, qreal startAngle,
        const QVector< QskGradientStop >& );

    QPointF center() const noexcept;
    void setCenter(const QPointF& center) noexcept;
    void setCenter(qreal x, qreal y) noexcept;

    qreal startAngle() const noexcept;
    void setStartAngle( qreal ) noexcept;

    qreal spanAngle() const noexcept;
    void setSpanAngle( qreal ) noexcept;
};

inline QskConicGradient::QskConicGradient() noexcept
    : QskConicGradient( Conic, 0.5, 0.5, 0.0, 360.0 )
{
}

inline QskConicGradient::QskConicGradient(
        const QPointF& center, qreal startAngle, qreal spanAngle ) noexcept
    : QskConicGradient( center.x(), center.y(), startAngle, spanAngle )
{
}

inline QskConicGradient::QskConicGradient(
        qreal cx, qreal cy, qreal startAngle, qreal spanAngle ) noexcept
    : QskGradient( Conic, cx, cy, startAngle, spanAngle )
{
}

inline QskConicGradient::QskConicGradient( const QPointF& center,
        qreal startAngle, const QColor& color1, const QColor& color2 )
    : QskConicGradient( center.x(), center.y(), startAngle, color1, color2 )
{
}

inline QskConicGradient::QskConicGradient( const QPointF& center,
        qreal startAngle, const QVector< QskGradientStop >& stops )
    : QskConicGradient( center.x(), center.y(), startAngle, stops )
{
}

inline QskConicGradient::QskConicGradient( const QPointF& center,
        qreal startAngle, QGradient::Preset preset )
    : QskConicGradient( center.x(), center.y(), startAngle, preset )
{
}

inline QPointF QskConicGradient::center() const noexcept
{
    return QPointF( m_values[0], m_values[1] );
}

inline qreal QskConicGradient::startAngle() const noexcept
{
    return m_values[2];
}

inline qreal QskConicGradient::spanAngle() const noexcept
{
    return m_values[3];
}

#endif
