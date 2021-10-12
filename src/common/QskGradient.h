/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_GRADIENT_H
#define QSK_GRADIENT_H

#include "QskGlobal.h"
#include "QskGradientStop.h"

#include <qcolor.h>
#include <qmetatype.h>
#include <qvector.h>

class QVariant;

typedef QVector< QskGradientStop > QskGradientStops;

class QSK_EXPORT QskGradient
{
    Q_GADGET

    Q_PROPERTY( Orientation orientation READ orientation WRITE setOrientation )
    Q_PROPERTY( QskGradientStops stops READ stops WRITE setStops )

    Q_PROPERTY( bool valid READ isValid )
    Q_PROPERTY( bool visible READ isVisible )
    Q_PROPERTY( bool monochrome READ isMonochrome )

  public:
    // TODO: radial/canonical gradients + other diagonal linear gradients
    enum Orientation
    {
        Horizontal,
        Vertical,
        Diagonal,
        Radial,
        Conical
    };

    Q_ENUM( Orientation )

    QskGradient();
    QskGradient( Qt::GlobalColor );
    QskGradient( QRgb );
    QskGradient( const QColor& );

    QskGradient( Qt::Orientation, const QskGradientStops& );
    QskGradient( Qt::Orientation, const QColor&, const QColor& );

    QskGradient( Orientation, const QskGradientStops& );
    QskGradient( Orientation, const QColor&, const QColor& );

    ~QskGradient();

    void setOrientation( Qt::Orientation );
    void setOrientation( Orientation );
    Orientation orientation() const;

    bool isValid() const;
    Q_INVOKABLE void invalidate();

    bool operator==( const QskGradient& ) const;
    bool operator!=( const QskGradient& ) const;

    void setColor( const QColor& );
    void setColors( const QColor&, const QColor& );

    Q_INVOKABLE QColor startColor() const;
    Q_INVOKABLE QColor endColor() const;

    Q_INVOKABLE void setStops( const QskGradientStops& );
    Q_INVOKABLE QskGradientStops stops() const;

    Q_INVOKABLE bool hasStopAt( qreal value ) const;

    void setAlpha( int alpha );

    bool isMonochrome() const;
    bool isVisible() const;

    void reverse();
    QskGradient reversed() const;

    // all stops between [from, to] with positions streched into [0,1]
    QskGradient extracted( qreal from, qreal start ) const;

    QskGradient interpolated( const QskGradient&, qreal value ) const;

    static QVariant interpolate( const QskGradient&,
        const QskGradient&, qreal progress );

    uint hash( uint seed ) const;

    Q_INVOKABLE qreal stopAt( int index ) const;
    Q_INVOKABLE QColor colorAt( int index ) const;
    Q_INVOKABLE int stopCount() const;

  private:
    void setStopAt( int index, qreal stop );
    void setColorAt( int index, const QColor& color );

    Orientation m_orientation;
    QskGradientStops m_stops;
};

Q_DECLARE_METATYPE( QskGradient )

inline QskGradient::QskGradient( Qt::GlobalColor color )
    : QskGradient( QColor( color ) )
{
}

inline QskGradient::QskGradient( QRgb rgb )
    : QskGradient( QColor::fromRgba( rgb ) )
{
}

inline QColor QskGradient::startColor() const
{
    return ( m_stops.size() >= 2 ) ? m_stops.first().color() : QColor();
}

inline QColor QskGradient::endColor() const
{
    return ( m_stops.size() >= 2 ) ? m_stops.last().color() : QColor();
}

inline bool QskGradient::operator==( const QskGradient& other ) const
{
    return ( m_orientation == other.m_orientation ) && ( m_stops == other.m_stops );
}

inline bool QskGradient::operator!=( const QskGradient& other ) const
{
    return ( !( *this == other ) );
}

#ifndef QT_NO_DEBUG_STREAM

class QDebug;

QSK_EXPORT QDebug operator<<( QDebug, const QskGradient& );

#endif

#endif
