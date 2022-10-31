/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_LINEAR_GRADIENT_H
#define QSK_LINEAR_GRADIENT_H

#include "QskGradient.h"

class QSK_EXPORT QskLinearGradient : public QskGradient
{
    Q_GADGET

    Q_PROPERTY( QPointF start READ start WRITE setStart )
    Q_PROPERTY( QPointF stop READ stop WRITE setStop )

  public:
    QskLinearGradient() noexcept;

    QskLinearGradient( Qt::GlobalColor );
    QskLinearGradient( QRgb );
    QskLinearGradient( const QColor& );
    QskLinearGradient( const QColor&, const QColor& );
    QskLinearGradient( QGradient::Preset );
    QskLinearGradient( const QVector< QskGradientStop >& );

    QskLinearGradient( Qt::Orientation ) noexcept;
    QskLinearGradient( Qt::Orientation, Qt::GlobalColor );
    QskLinearGradient( Qt::Orientation, QRgb );
    QskLinearGradient( Qt::Orientation, const QColor& );
    QskLinearGradient( Qt::Orientation, const QColor&, const QColor& );
    QskLinearGradient( Qt::Orientation, QGradient::Preset );
    QskLinearGradient( Qt::Orientation, const QVector< QskGradientStop >& );

    QskLinearGradient( const QPointF&, const QPointF& ) noexcept;
    QskLinearGradient( qreal x1, qreal y1, qreal x2, qreal y2 ) noexcept;

    QskLinearGradient( const QPointF&, const QPointF&,
        const QColor&, const QColor& );

    QskLinearGradient( qreal x1, qreal y1,
        qreal x2, qreal y2, const QColor&, const QColor& );

    QskLinearGradient( const QPointF&, const QPointF&, QGradient::Preset );

    QskLinearGradient( qreal x1, qreal y1,
        qreal x2, qreal y2, QGradient::Preset );

    QskLinearGradient( const QPointF&, const QPointF&,
        const QVector< QskGradientStop >& );

    QskLinearGradient( qreal x1, qreal y1,
        qreal x2, qreal y2, const QVector< QskGradientStop >& );

    void setStart(const QPointF& start) noexcept;
    void setStart( qreal x, qreal y ) noexcept;

    void setStop(const QPointF& start) noexcept;
    void setStop( qreal x, qreal y ) noexcept;

    void setInterval( Qt::Orientation, qreal, qreal );

    void setOrientation( Qt::Orientation ) noexcept;
    bool isOriented( Qt::Orientation ) const noexcept;

    bool isHorizontal() const noexcept;
    bool isVertical() const noexcept;
    bool isTilted() const noexcept;

    QPointF start() const noexcept;
    QPointF stop() const noexcept;

    qreal x1() const noexcept;
    qreal y1() const noexcept;

    qreal x2() const noexcept;
    qreal y2() const noexcept;
};

inline QskLinearGradient::QskLinearGradient() noexcept
    : QskGradient( QskGradient::Linear, 0.0, 0.0, 0.0, 1.0 )
{
}

inline QskLinearGradient::QskLinearGradient( Qt::GlobalColor color )
    : QskLinearGradient( QColor( color ) )
{
}

inline QskLinearGradient::QskLinearGradient( QRgb rgb )
    : QskLinearGradient( QColor::fromRgba( rgb ) )
{
}

inline QskLinearGradient::QskLinearGradient(
        Qt::Orientation orientation, Qt::GlobalColor color )
    : QskLinearGradient( orientation, QColor( color ) )
{
}

inline QskLinearGradient::QskLinearGradient(
        Qt::Orientation orientation, QRgb rgb )
    : QskLinearGradient( orientation, QColor::fromRgba( rgb ) )
{
}

inline QskLinearGradient::QskLinearGradient(
        const QPointF& start, const QPointF& stop,
        const QColor& color1, const QColor& color2 )
    : QskLinearGradient( start.x(), start.y(), stop.x(), stop.y(), color1, color2 )
{
}

inline QskLinearGradient::QskLinearGradient(
        const QPointF& start, const QPointF& stop, QGradient::Preset preset )
    : QskLinearGradient( start.x(), start.y(), stop.x(), stop.y(), preset )
{
}

inline QskLinearGradient::QskLinearGradient(
        const QPointF& start, const QPointF& stop,
        const QVector< QskGradientStop >& stops )
    : QskLinearGradient( start.x(), start.y(), stop.x(), stop.y(), stops )
{
}

inline QskLinearGradient::QskLinearGradient(
        const QPointF& start, const QPointF& stop ) noexcept
    : QskLinearGradient( start.x(), start.y(), stop.x(), stop.y() )
{
}

inline QskLinearGradient::QskLinearGradient(
        qreal x1, qreal y1, qreal x2, qreal y2 ) noexcept
    : QskGradient( QskGradient::Linear, x1, y1, x2, y2 )
{
}

inline qreal QskLinearGradient::x1() const noexcept
{
    return m_values[0];
}

inline qreal QskLinearGradient::y1() const noexcept
{
    return m_values[1];
}

inline qreal QskLinearGradient::x2() const noexcept
{
    return m_values[2];
}

inline qreal QskLinearGradient::y2() const noexcept
{
    return m_values[3];
}

inline QPointF QskLinearGradient::start() const noexcept
{
    return QPointF( x1(), y1() );
}

inline QPointF QskLinearGradient::stop() const noexcept
{
    return QPointF( x2(), y2() );
}

inline bool QskLinearGradient::isOriented(
    Qt::Orientation orientation ) const noexcept
{
    return ( orientation == Qt::Horizontal )
        ? isHorizontal() : isVertical();
}

inline bool QskLinearGradient::isHorizontal() const noexcept
{
    return !isVertical() && ( y1() == y2() );
}

inline bool QskLinearGradient::isVertical() const noexcept
{
    return x1() == x2();
}

inline bool QskLinearGradient::isTilted() const noexcept
{
    return ( x1() != x2() ) && ( y1() != y2() );
}

#endif
