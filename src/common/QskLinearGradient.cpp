/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#include "QskLinearGradient.h"

QskLinearGradient::QskLinearGradient( Qt::Orientation orientation ) noexcept
    : QskGradient( QskGradient::Linear )
{
    setOrientation( orientation );
}

QskLinearGradient::QskLinearGradient( const QColor& color )
    : QskLinearGradient( Qt::Vertical )
{
    setStops( color );
}

QskLinearGradient::QskLinearGradient(
        const QColor& startColor, const QColor& stopColor )
    : QskLinearGradient( Qt::Vertical )
{
    setStops( startColor, stopColor );
}

QskLinearGradient::QskLinearGradient( QGradient::Preset preset )
    : QskLinearGradient( Qt::Vertical )
{
    setStops( preset );
}

QskLinearGradient::QskLinearGradient( const QVector< QskGradientStop >& stops )
    : QskLinearGradient( Qt::Vertical )
{
    setStops( stops );
}

QskLinearGradient::QskLinearGradient( Qt::Orientation orientation, const QColor& color )
    : QskLinearGradient( orientation )
{
    setStops( color );
}

QskLinearGradient::QskLinearGradient( Qt::Orientation orientation,
        const QColor& startColor, const QColor& stopColor )
    : QskLinearGradient( orientation )
{
    setStops( startColor, stopColor );
}

QskLinearGradient::QskLinearGradient(
        Qt::Orientation orientation, QGradient::Preset preset )
    : QskLinearGradient( orientation )
{
    setStops( preset );
}

QskLinearGradient::QskLinearGradient(
        Qt::Orientation orientation, const QVector< QskGradientStop >& stops )
    : QskLinearGradient( orientation )
{
    setStops( stops );
}

void QskLinearGradient::setOrientation( Qt::Orientation orientation  ) noexcept
{
    setStart( 0.0, 0.0 );

    if ( orientation == Qt::Vertical )
        setStop( 0.0, 1.0 );
    else
        setStop( 1.0, 0.0 );
}

QskLinearGradient::QskLinearGradient( qreal x1, qreal y1,
        qreal x2, qreal y2, const QColor& color1, const QColor& color2 )
    : QskLinearGradient( x1, y1, x2, y2 )
{
    setStops( color1, color2 );
}

QskLinearGradient::QskLinearGradient( qreal x1, qreal y1,
        qreal x2, qreal y2, QGradient::Preset preset )
    : QskLinearGradient( x1, y1, x2, y2 )
{
    setStops( preset );
}

QskLinearGradient::QskLinearGradient( qreal x1, qreal y1,
        qreal x2, qreal y2, const QVector< QskGradientStop >& stops )
    : QskLinearGradient( x1, y1, x2, y2 )
{
    setStops( stops );
}

void QskLinearGradient::setStart( const QPointF& pos ) noexcept
{
    m_values[0] = pos.x();
    m_values[1] = pos.y();
}

void QskLinearGradient::setStart( qreal x, qreal y ) noexcept
{
    m_values[0] = x;
    m_values[1] = y;
}

void QskLinearGradient::setStop( const QPointF& pos ) noexcept
{
    m_values[2] = pos.x();
    m_values[3] = pos.y();
}

void QskLinearGradient::setStop( qreal x, qreal y ) noexcept
{
    m_values[2] = x;
    m_values[3] = y;
}

void QskLinearGradient::setInterval( Qt::Orientation orientation, qreal from, qreal to )
{
    if ( orientation == Qt::Vertical )
    {
        m_values[1] = from;
        m_values[3] = to;
    }
    else
    {
        m_values[0] = from;
        m_values[2] = to;
    }
}

#include "moc_QskLinearGradient.cpp"
