/******************************************************************************
 * QSkinny - Copyright (C) 2016 Uwe Rathmann
 * This file may be used under the terms of the QSkinny License, Version 1.0
 *****************************************************************************/

#ifndef QSK_GRADIENT_H
#define QSK_GRADIENT_H

#include "QskGlobal.h"

#include <qcolor.h>
#include <qmetatype.h>
#include <qvector.h>

class QDebug;
class QVariant;

class QSK_EXPORT QskGradientStop
{
    Q_GADGET

    Q_PROPERTY( qreal position READ position WRITE setPosition RESET resetPosition )
    Q_PROPERTY( QColor color READ color WRITE setColor RESET resetColor )

  public:
    QskGradientStop();
    QskGradientStop( qreal position, const QColor& color );

    bool operator==( const QskGradientStop& ) const;
    bool operator!=( const QskGradientStop& ) const;

    void setStop( qreal position, const QColor& color );

    qreal position() const;
    void setPosition( qreal position );
    void resetPosition();

    const QColor& color() const;
    void setColor( const QColor& color );
    void resetColor();

    static QColor interpolated(
        const QskGradientStop&, const QskGradientStop&, qreal position );

    uint hash( uint seed ) const;

  private:
    qreal m_position;
    QColor m_color;
};

class QSK_EXPORT QskGradient
{
    Q_GADGET

    Q_PROPERTY( Orientation orientation READ orientation WRITE setOrientation )
    Q_PROPERTY( QVector< QskGradientStop > stops READ stops WRITE setStops )

    Q_PROPERTY( bool valid READ isValid )
    Q_PROPERTY( bool visible READ isVisible )
    Q_PROPERTY( bool monochrome READ isMonochrome )

  public:
    // TODO: radial/canonical gradients + other diagonal linear gradients
    enum Orientation
    {
        Horizontal,
        Vertical,
        Diagonal
    };

    Q_ENUM( Orientation )

    QskGradient();
    QskGradient( Qt::GlobalColor );
    QskGradient( QRgb );
    QskGradient( const QColor& );

    QskGradient( Qt::Orientation, const QVector< QskGradientStop >& );
    QskGradient( Qt::Orientation, const QColor&, const QColor& );

    QskGradient( Orientation, const QVector< QskGradientStop >& );
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

    Q_INVOKABLE void setStops( const QVector< QskGradientStop >& );
    Q_INVOKABLE QVector< QskGradientStop > stops() const;

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
    QVector< QskGradientStop > m_stops;
};

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

inline QskGradientStop::QskGradientStop()
    : m_position( -1.0 )
{
}

inline QskGradientStop::QskGradientStop( qreal position, const QColor& color )
    : m_position( position )
    , m_color( color )
{
}

inline qreal QskGradientStop::position() const
{
    return m_position;
}

inline const QColor& QskGradientStop::color() const
{
    return m_color;
}

inline bool QskGradientStop::operator==( const QskGradientStop& other ) const
{
    return ( m_position == other.m_position ) && ( m_color == other.m_color );
}

inline bool QskGradientStop::operator!=( const QskGradientStop& other ) const
{
    return ( !( *this == other ) );
}

inline bool QskGradient::operator==( const QskGradient& other ) const
{
    return ( m_orientation == other.m_orientation ) && ( m_stops == other.m_stops );
}

inline bool QskGradient::operator!=( const QskGradient& other ) const
{
    return ( !( *this == other ) );
}

Q_DECLARE_TYPEINFO( QskGradientStop, Q_MOVABLE_TYPE );

Q_DECLARE_METATYPE( QskGradientStop )
Q_DECLARE_METATYPE( QskGradient )

#ifndef QT_NO_DEBUG_STREAM

QSK_EXPORT QDebug operator<<( QDebug, const QskGradientStop& );
QSK_EXPORT QDebug operator<<( QDebug, const QskGradient& );

#endif

#endif
